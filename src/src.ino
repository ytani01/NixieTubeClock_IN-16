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

std::string VersionString = " 2. 1. 5";

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
void cbBtn(ButtonInfo_t *bi, std::map<std::string, ButtonInfo_t>& btn_info) {
  std::string btn_val_str = "";
  for (auto b: btn_info) {
    btn_val_str += " [" + b.first + "]:";
    btn_val_str += (b.second.value == Button::ON ? "ON " : "OFF");
  }
  log_i("%s,%s", Button::info2String(bi).c_str(), btn_val_str.c_str());

  //
  // reboot check
  //
  if ( btn_info["Btn0"].value == Button::ON &&
       btn_info["Btn2"].value == Button::ON) {
    Mode::set("ModeReboot");
    return;
  } // if (Btn0 + Btn2)

  Mode::Cur->cbBtn(bi, btn_info);
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
    DateTime prev_dt = Rtc->now();
    std::string prev_dt_string = datetime2string(&prev_dt);

    struct tm *tm_sys = SysClock::now_tm();
    std::string tm_sys_string = tm2string(tm_sys);

    Rtc->adjust(tm_sys);

    DateTime now_dt = Rtc->now();
    std::string now_dt_string = datetime2string(&now_dt);

    if ( prev_dt_string != now_dt_string ) {
      log_i("== adjust RTC from NTP: now RTC     : %s",
            now_dt_string.c_str());
      log_i("                        src Sys(NTP): %s",
            tm2string(tm_sys).c_str());
      log_i("                        dst RTC     : %s",
            datetime2string(&now_dt).c_str());
    }
  } else {
    //
    // NTP is not available
    //
    // adjust system clock from RTC
    //
    std::string prev_sys_string = SysClock::now_string();

    DateTime now_dt = Rtc->now();
    std::string now_rtc_string = datetime2string(&now_dt);

    SysClock::set(&now_dt);
    std::string now_sys_string = SysClock::now_string();

    if ( prev_sys_string != now_sys_string ) {
      log_i("== adjust system clock from RTC: src RTC: %s",
            now_rtc_string.c_str());
      log_i("                                 dst Sys: %s",
            now_sys_string.c_str());
    }
  } // if (SNTP_SYNC_STATUS_COMPLETED)

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
  //log_i("=== uxTaskGetStackHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));

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

  // enable interrupt
  enableIntr();

  // Heap check
  log_i("=== MAC address: %s ===",
        get_mac_addr_string().c_str());
  log_i("========== CORE_DEBUG_LEVEL = %d\n", CORE_DEBUG_LEVEL);

  std::deque<uint32_t> heap = chk_heap(5);
  std::string heap_str = "";

  for (auto h: heap) {
    heap_str += " " + std::to_string(h);
  }

  log_i("========== heap:%s", heap_str.c_str());

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
#if 1
  static unsigned long prev_ms = 0;
  unsigned long ms = millis();

  static unsigned long interval = 2 * 60 * 1000;
  if ( ms - prev_ms > interval ) {
    std::deque<uint32_t> heap = chk_heap(5);
    std::string heap_str = "";

    for (auto h: heap) {
      heap_str += " " + std::to_string(h);
    }

    unsigned long prev_interval = interval;
    
    if ( heap.size() >= 2 ) {
      if ( heap[0] != heap[1] ) {
        interval /= 2;
      } else {
        interval *= 2;
      }
    }

    log_i("========== %s> heap:%s, interval: %u -> %u",
          SysClock::now_string().c_str(),
          heap_str.c_str(), prev_interval, interval);
    
    prev_ms = ms;
  } // if(interval)
#endif

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
    auto xLastTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastTime, 1);
  } else {
    log_e("Mode::Cur == NULL !?");
    delayOrChangeMode(2000);
  }
} // loop()
