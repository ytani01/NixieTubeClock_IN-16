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
#include "Mode_Boot.h"
#include "Mode_Reboot.h"
#include "Mode_Clock.h"
#include "Mode_Setclock.h"
#include "Mode_Scoreboard.h"
#include "Mode_Test1.h"

std::string VersionString = " 2. 2. 0";

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

bool Flag_SkipCbBtn = false;

// WiFi
static const std::string AP_HDR = "NxClk_";
Task_WifiMgr *TaskWifiMgr = NULL;

// Ntp
//const String NTP_SVR[] = {"ntp.nict.jp", "pool.ntp.org", "time.google.com"};
static const std::vector<std::string> NTP_SVR = {
  "ntp.nict.jp",
  "pool.ntp.org",
  "time.google.com"
};

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

Nixie_TubeArray *Nxa = NULL;
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
bool skipCbBtn(const ButtonInfo_t& bi,
               const std::map<std::string, ButtonInfo_t>& btn_info) {
  if ( ! Flag_SkipCbBtn ) return false;

  /*  Flag_SkipCbBtn == true  */
  if ( bi.value == Button::ON ) {

    int count = 0;
    for (auto b: btn_info) {
      if ( b.second.value == Button::ON ) {
        count++;
        if ( count >= 2 ) return true;
      }
    } // for (btn_info)

    if ( bi.push_count > 1 ) return true;
    if ( bi.long_pressed ) return true;

    Flag_SkipCbBtn = false;
    log_i("Flag_SkipCbBtn = %s", Flag_SkipCbBtn ? "true" : "false");
    return false;
  } // if(ON)

  /*  OFF  */

  int count = 0;
  for (auto b: btn_info) {
    if ( b.second.value == Button::ON ) return true;
  } // for (btn_info)

  /*  all Buttons are OFF  */
  Flag_SkipCbBtn = false;
  log_i("Flag_SkipCbBtn = %s", Flag_SkipCbBtn ? "true" : "false");

  return true;
} // skipCbBtn()

/**
 *
 */
void cbBtn(const ButtonInfo_t& bi,
           const std::map<std::string, ButtonInfo_t>& btn_info) {
  /*
    for debug
  */
  std::string btn_val_str = "";
  for (auto b: btn_info) {
    btn_val_str += " [" + b.first + "] ";
    btn_val_str += (b.second.value == Button::ON ? "ON  " : "OFF ");
  }
  log_i("%s,%s",
        Button::info2String(const_cast<ButtonInfo_t*>(&bi)).c_str(),
        btn_val_str.c_str());

  /*
    skip callback ?
  */
  if ( skipCbBtn(bi, btn_info) ) {
    log_w(".. ignored");
    return;
  }
  
  /*
    reboot check
  */
  // ### memo ###
  // const std::mapの場合、foo[key]ではなく、foo.at(key)で参照する。
  // foo[key]は、std::mapを書き換える可能性がある書式だから(?)
  //
  if ( btn_info.at("Btn0").value == Button::ON &&
       btn_info.at("Btn2").value == Button::ON) {
    Mode::set("Mode_Reboot");
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

    struct tm *now_sys = SysClock::now_tm();
    std::string now_sys_string = tm2string(now_sys);

    Rtc->adjust(now_sys);

    DateTime now_rtc = Rtc->now();
    std::string now_rtc_string = datetime2string(&now_rtc);

    if ( prev_dt_string != now_rtc_string ) {
      log_i("== adjust RTC from NTP");
      log_i("  prev RTC     : %s", prev_dt_string.c_str());
      log_i("  src  Sys(NTP): %s", now_sys_string.c_str());
      log_i("  dst  RTC     : %s", now_rtc_string.c_str());
    }
  } else {
    //
    // NTP is not available
    //
    // adjust system clock from RTC
    //
    std::string prev_sys_string = SysClock::now_string();

    DateTime now_rtc = Rtc->now();
    std::string now_rtc_string = datetime2string(&now_rtc);

    SysClock::set(&now_rtc);
    std::string now_sys_string = SysClock::now_string();

    if ( prev_sys_string != now_sys_string ) {
      log_i("== adjust system clock from RTC");
      log_i("  prev Sys: %s", prev_sys_string.c_str());
      log_i("  src  RTC: %s", now_rtc_string.c_str());
      log_i("  dst  Sys: %s", now_sys_string.c_str());
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

  // Nixie_Tube
  log_i("=== Nixie Tube Array");
  Nxa = new Nixie_TubeArray(PIN_HV5812_CLK,  PIN_HV5812_STOBE,
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
  TaskNtp = new Task_Ntp(NTP_SVR, cbNtp);
  TaskNtp->start();
  delay(100);
  
  // Mode
  log_i("=== Init Modes");
  Mode::add("Mode_Boot", new Mode_Boot(2500));
  Mode::add("Mode_Reboot", new Mode_Reboot(2000));
  Mode::add("Mode_Clock", new Mode_Clock());
  Mode::add("Mode_Setclock", new Mode_Setclock());
  Mode::add("Mode_Scoreboard", new Mode_Scoreboard());
  Mode::add("Mode_Test1", new Mode_Test1());

  for (auto m: Mode::Ent) {
    m.second->setup();
  }

  Mode::set("Mode_Boot");
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
  /*
    heap check
  */
  static unsigned long prev_ms = 0;
  unsigned long ms = millis();

  static unsigned long interval = 1 * 60 * 1000;
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

    log_i("========== %s > heap:%s, interval: %s -> %s",
          SysClock::now_string().c_str(), heap_str.c_str(),
          ms2string(prev_interval).c_str(), ms2string(interval).c_str());
    
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
