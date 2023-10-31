/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <map>
#include <esp32-hal-log.h>

#include "SysClock.h"
#include "MyRtc.h"
#include "Display.h"

#include "Task_NixieTubeArray.h"
#include "Task_ButtonWatcher.h"
#include "Task_WifiMgr.h"
#include "Task_Ntp.h"

#include "Mode.h"
#include "ModeBoot.h"
#include "ModeReboot.h"
#include "ModeClock.h"
#include "ModeSetclock.h"
#include "ModeScoreboard.h"

std::string VersionString = " 2. 1. 2";

// Mode
bool Flag_ReqModeChange = false; // モード変更要求フラグ
bool Flag_LoopRunning = false;   // モード内 loop() 実行中フラグ

// I2C
#define PIN_I2C_SDA 8
#define PIN_I2C_SCL 9

// RTC
MyRtc *Rtc;

// OLED Display
Display_t *Disp;

// Buttons
static const std::map<const char *, uint8_t> PIN_BTN = {
  {"Btn0", 14},
  {"Btn1", 17},
  {"Btn2", 18},
};
Task_ButtonWatcher *TaskBtnWatcher = NULL;

static const int BTN_REPEAT_COUNT_REBOOT = 15;

// WiFi
static const std::string AP_HDR = "NxClk_";
Task_WifiMgr *TaskWifiMgr = NULL;

// Ntp
const String NTP_SVR[] = {"ntp.nict.jp", "pool.ntp.org", "time.google.com"};
Task_Ntp *TaskNtp = NULL;

// Nixie Tube
#define PIN_HV5812_CLK      7
#define PIN_HV5812_STOBE    5
#define PIN_HV5812_DATA     6
#define PIN_HV5812_BLANK    4

#define PIN_COLON_L_TOP     2
#define PIN_COLON_L_BOTTOM  2
#define PIN_COLON_R_TOP     1
#define PIN_COLON_R_BOTTOM  1

uint8_t PINS_NIXIE_NUM[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N] = {
  { 9,  0,  6,  2,  3,  4,  5,  1,  7,  8},
  {15, 14, 18, 12, 11, 10, 19, 13, 17, 16},
  {35, 20, 38, 22, 23, 24, 39, 21, 37, 36},
  {29, 34, 26, 32, 31, 30, 25, 33, 27, 28},
  {55, 54, 58, 52, 51, 50, 59, 53, 57, 56},
  {49, 40, 46, 42, 43, 44, 45, 41, 47, 48}
};
uint8_t PINS_NIXIE_COLON[NIXIE_COLON_N][NIXIE_COLON_DOT_N] = {
  {PIN_COLON_R_TOP},
  {PIN_COLON_L_TOP}
};

NixieTubeArray *Nxa = NULL;
Task_NixieTubeArray *TaskNixieTubeArray = NULL;

/** global for enableIntr()/disableIntr()
 *
 */
bool Flag_enableIntr = false;

/** global
 *
 */
void enableIntr() {
  if ( TaskBtnWatcher ) {
    TaskBtnWatcher->enable();
  }
  Flag_enableIntr = true;
}

/** global
 *
 */
void disableIntr() {
  if ( TaskBtnWatcher ) {
    TaskBtnWatcher->disable();
  }
  Flag_enableIntr = false;
}

/**
 *
 */
void cbBtn(ButtonInfo_t *bi, std::map<std::string, bool>& btn_val) {
  std::string btn_val_str = "";
  for (auto b: btn_val) {
    btn_val_str += " [" + b.first + "]:" + (b.second ? "OFF" : "ON ");
  }
  log_i("%s,%s", Button::info2String(bi).c_str(), btn_val_str.c_str());

  //
  // reboot check
  //
#if 0
  if ( String(bi->name) == "Btn0" &&
       bi->value == Button::ON &&
       bi->long_pressed &&
       bi->repeat_count > BTN_REPEAT_COUNT_REBOOT ) {
#endif
  if ( btn_val["Btn0"] == Button::ON && btn_val["Btn2"] == Button::ON) {
    Mode::set("ModeReboot");
    return;
  } // if (Btn0 + Btn2)


  Mode::Cur->cbBtn(bi, btn_val);
} // cbBtn()

/**
 *
 */
void cbNtp(Task_NtpInfo_t *ni) {
  sntp_sync_status_t prev_sntp_stat = SNTP_SYNC_STATUS_RESET;

  log_d("%s", SNTP_SYNC_STATUS_STR[ni->sntp_stat]);

  if ( ni->sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
    //
    // adjust RTC
    //
    log_i("== adjust RTC from NTP");

    DateTime now_dt = Rtc->now();
    log_i("  now RTC     : %s",
          datetime2string(&now_dt, "%Y-%m-%d(%a),%H:%M:%S").c_str());

    struct tm *tm_sys = SysClock::now_tm();
    log_i("  src Sys(NTP): %s",
          tm2string(tm_sys, "%Y-%m-%d(%a),%H:%M:%S").c_str());

    Rtc->adjust(tm_sys);
    
    now_dt = Rtc->now();
    log_i("  dst RTC     : %s",
          datetime2string(&now_dt, "%Y-%m-%d(%a),%H:%M:%S").c_str());
  } else {
    //
    // NTP is not available
    //
    // adjust system clock from RTC
    //
    log_i("== adjust system clock from RTC");

    DateTime now_dt = Rtc->now();
    log_i("  src RTC: %s",
          datetime2string(&now_dt, "%Y-%m-%d(%a),%H:%M:%S").c_str());

    SysClock::set(&now_dt);

    log_i("  dst Sys: %s", SysClock::now_string().c_str());
  }

  prev_sntp_stat = ni->sntp_stat;
} // cbNtp()

/**
 *
 */
void setup() {
#if 0
  for (int i=0; i < 3; i++) {
    log_i("%d ===", i);
    delay(500);
  }
#endif
  log_i("=== uxTaskGetStackHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));

  // NixieTube
  log_i("=== Nixie Tube Array");
  Nxa = new NixieTubeArray(PIN_HV5812_CLK,  PIN_HV5812_STOBE,
                           PIN_HV5812_DATA, PIN_HV5812_BLANK,
                           PINS_NIXIE_NUM, PINS_NIXIE_COLON);
  //Nxa->set_string(VersionString);
  Nxa->set_string("        ");

  TaskNixieTubeArray = new Task_NixieTubeArray(Nxa, 0);
  TaskNixieTubeArray->start();
  delay(100);

  // I2C
  log_i("=== Init I2C: SDA=%d, SCL=%d", PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
  
  // Display
  Disp = new Display_t(DISPLAY_W, DISPLAY_H);
  Disp->DispBegin(DISP_ADDR);
  Disp->setRotation(0);
  Disp->clearDisplay();
  Disp->setCursor(0, 0);
  Disp->setTextSize(2);
  Disp->printf("%s", VersionString.c_str());
  Disp->display();
  delay(100);

  // set timezone
  setenv("TZ", "JST-9", 1);
  tzset();

  // RTC
  Rtc = new MyRtc();
  Rtc->begin(&Wire);
  DateTime dt_rtc = Rtc->now();
  log_i("=== RTC      : %s", datetime2string(&dt_rtc).c_str());

  // SysClock
  SysClock::set(&dt_rtc);
  log_i("=== SysClock : %s", SysClock::now_string().c_str());
  delay(100);

  // Button
  log_i("=== Init Buttons");
  TaskBtnWatcher = new Task_ButtonWatcher(cbBtn);

  for (auto btn: PIN_BTN) {
    log_i("{%s %d}", btn.first, btn.second);
    TaskBtnWatcher->addBtn(String(btn.first), btn.second);
  }

  TaskBtnWatcher->start();
  delay(100);

  // WiFi
  log_i("=== Init WiFi");
  TaskWifiMgr = new Task_WifiMgr(AP_HDR);
  TaskWifiMgr->start();
  delay(100);

  // NTP
  log_i("=== Init NTP");
  TaskNtp = new Task_Ntp((String *)NTP_SVR, cbNtp);
  TaskNtp->start();
  delay(100);
  
  // Mode
  log_i("=== Init Modes");
  Mode::add("ModeBoot", new ModeBoot(2500));
  Mode::add("ModeReboot", new ModeReboot(2000));
  Mode::add("ModeClock", new ModeClock());
  Mode::add("ModeSetclock", new ModeSetclock());
  Mode::add("ModeScoreboard", new ModeScoreboard());

  for (auto m: Mode::Ent) {
    m.second->setup();
  }

  Mode::set("ModeBoot");
  delay(100);

  enableIntr();

  log_i("=== uxTaskGetStackHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
  log_i("=== start %s ===",
        get_mac_addr_string().c_str());
} // setup()

/**
 *
 */
unsigned long delayOrChangeMode(unsigned long ms) {
  unsigned long ms_count = 0;
  unsigned long start_ms = millis();

  for ( ms_count=0; ms_count < ms && ! Flag_ReqModeChange; ms_count++ ) {
    delay(1);
  }
  return  millis() - start_ms;
} // delayOrChangeMode()

/**
 *
 */
void loop() {
  //log_v("class %s", __CLASS_NAME__.c_str());

  if ( Mode::Cur ) {
    Flag_LoopRunning = true;
    Mode::Cur->loop();
    if ( Flag_ReqModeChange ) {
      log_i("%s::loop(): done", Mode::Prev->name.c_str());
    }
    Flag_LoopRunning = false;

    /*
      モード変更要求 ``Flag_ReqModeChange`` があった場合、
      モード切替が完了するまで待つ
    */
    unsigned long wait_count = 0;
    while ( Flag_ReqModeChange ) {
      //delay(1);
      //delayMicroseconds(1); // X
      //ets_delay_us(1); // X
      wait_count++;
      auto xLastTime = xTaskGetTickCount();
      vTaskDelayUntil(&xLastTime, 1);
    }
    if ( wait_count > 0 ) {
      log_i("wait_count=%u", wait_count);
    }
  } else {
    log_e("Mode::Cur == NULL !?");
    delayOrChangeMode(2000);
  }
} // loop()
