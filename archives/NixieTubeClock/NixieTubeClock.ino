/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Nixie.h"
#include "Button.h"
#include "NetMgr.h"
#include "ModeBase.h"
#include "ModeClock.h"
#include "ModeSetClock.h"
#include "ModeCount.h"
#include "ModeTest1.h"
#include "ModeTest2.h"
#include "esp_sntp.h"

static const String MY_NAME = "Nixie Tube Clock";
/**
 * Version X.Y.Z
 * {v0,v1, v2,v3, v4,v5}
 *
 * X = v0 * 10 + v1
 * Y = v2
 * Z = v3
 * v4, v5: 0 (always)
 */
int                 initValVer[NIXIE_NUM_N] = {0,1, 0,3, 0,3};

#define LOOP_DELAY_US   1   // micro sbeconds
#define DEBOUNCE        300 // msec

String WDayStr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

//======================================================================
// pins
//----------------------------------------------------------------------
#define PIN_I2C_SDA         8
#define PIN_I2C_SCL         9

#define PIN_HV5812_CLK      7
#define PIN_HV5812_STOBE    5
#define PIN_HV5812_DATA     6
#define PIN_HV5812_BLANK    4

#define PIN_COLON_R_TOP     2
#define PIN_COLON_R_BOTTOM  2
#define PIN_COLON_L_TOP     1
#define PIN_COLON_L_BOTTOM  1

#define PIN_BTN0           14
#define PIN_BTN1           17
#define PIN_BTN2           18


uint8_t pinsIn[] = {PIN_BTN0, PIN_BTN1, PIN_BTN2};

uint8_t nixiePins[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N] =
  {
   { 9,  0,  6,  2,  3,  4,  5,  1,  7,  8},
   {15, 14, 18, 12, 11, 10, 19, 13, 17, 16},
   {35, 20, 38, 22, 23, 24, 39, 21, 37, 36},
   {29, 34, 26, 32, 31, 30, 25, 33, 27, 28},
   {55, 54, 58, 52, 51, 50, 59, 53, 57, 56},
   {49, 40, 46, 42, 43, 44, 45, 41, 47, 48}
  };

uint8_t colonPins[NIXIE_COLON_N][NIXIE_COLON_DOT_N] =
  {
   {PIN_COLON_R_TOP},
   {PIN_COLON_L_TOP}
  };

uint8_t intrPin0 = digitalPinToInterrupt(PIN_BTN0);
uint8_t intrPin1 = digitalPinToInterrupt(PIN_BTN1);
uint8_t intrPin2 = digitalPinToInterrupt(PIN_BTN2);

//======================================================================
// NetMgr
//----------------------------------------------------------------------
NetMgr  netMgr;
boolean wifiActive      = false;
boolean prev_wifiActive = false;

//======================================================================
// NTP
//----------------------------------------------------------------------
const String        NTP_SVR[]    = {"pool.ntp.org",
                                    "ntp.nict.jp",
                                    "time.google.com"};
const unsigned long NTP_INTERVAL = 1 * 60 * 1000; // msec
unsigned long       ntpLast      = 0;
boolean             ntpActive    = false;
static const char *SNTP_SYNC_STATUS_STR[] = {
  "RESET",
  "COMPLETED",
  "IN_PROGRESS"
};

//======================================================================
// RTC DS3231
//----------------------------------------------------------------------
//RTC_DS3231 Rtc;
RTC_PCF8563 Rtc;

//======================================================================
// Nixie Array
//----------------------------------------------------------------------
NixieArray nixieArray(PIN_HV5812_CLK,  PIN_HV5812_STOBE,
                      PIN_HV5812_DATA, PIN_HV5812_BLANK,
                      nixiePins, colonPins);
//======================================================================
// Modes
//----------------------------------------------------------------------
ModeClock modeClock = ModeClock(&nixieArray);
ModeTest1 modeTest1 = ModeTest1(&nixieArray);
ModeTest2 modeTest2 = ModeTest2(&nixieArray);
ModeSetClock modeSetClock = ModeSetClock(&nixieArray);
ModeCount modeCount = ModeCount(&nixieArray);

ModeBase* Mode[] = {&modeClock, &modeSetClock, &modeCount, &modeTest1, &modeTest2};
const unsigned long MODE_N = sizeof(Mode) / sizeof(&Mode[0]);
const unsigned int MODE_CLOCK     = 0;
const unsigned int MODE_SET_CLOCK = 1;
const unsigned int MODE_COUNT     = 2;
const unsigned int MODE_TEST1     = 3;
const unsigned int MODE_TEST2     = 4;

long curMode = 0;
long prevMode = -1;

//======================================================================
// Buttons
//----------------------------------------------------------------------
Button b0 = Button(PIN_BTN0, "BTN0");
Button b1 = Button(PIN_BTN1, "BTN1");
Button b2 = Button(PIN_BTN2, "BTN2");

Button* Btn[] = {&b0, &b1, &b2};

const unsigned long BTN_N = sizeof(Btn) / sizeof(&Btn[0]);

//======================================================================
// global variables
//----------------------------------------------------------------------
unsigned long loopCount = 0; // XXX tobe local variable in loop()
unsigned long curMsec   = 0; // msec XXX tobe local variable in loop()
unsigned long prevMsec  = 0; // msec XXX tobe local variable in loop()

//======================================================================
/**
 *
 */
char *tm2str(struct tm *tm) {
  static char str[64];

  int res = strftime(str, sizeof(str), "%Y/%m/%d(%a) %H:%M:%S", tm);

  return (char *)str;
}

char *datetime2str(DateTime *dt) {
  static char str[64];

  sprintf(str, "%04d/%02d/%02d(%s) %02d:%02d:%02d",
          dt->year(), dt->month(), dt->day(),
          WDayStr[dt->dayOfTheWeek()],
          dt->hour(), dt->minute(), dt->second());

  return (char *)str;
}

/**
 *
 */
struct tm* get_internal_clock() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    return tm;
}

/**
 *
 */
void ntp_adjust() {
  if (! ntpActive) {
    return;
  }

  (void)get_internal_clock();

  disableIntr();
  /*
  configTime(9L * 3600L, 0,
             NTP_SVR[0].c_str(), NTP_SVR[1].c_str(), NTP_SVR[2].c_str());
  */
  configTzTime("JST-9",
               NTP_SVR[0].c_str(), NTP_SVR[1].c_str(), NTP_SVR[2].c_str());
  enableIntr();
  log_i("configTzTime(%s, %s, %s, %s)",
        "JST-9",
        NTP_SVR[0].c_str(),
        NTP_SVR[1].c_str(),
        NTP_SVR[2].c_str());
  log_i("SNTP_SYNC_STATUS=%s",
        SNTP_SYNC_STATUS_STR[sntp_get_sync_status()]);

  // adjust internal clock from RTC
  //
  // XXX
  // configTime(), configTzTime()実行直後(NTP同期完了前)、
  // 内部時計が狂うことがあるので(!?)、一旦 RTCに合わせる
  //
  DateTime now_rtc = Rtc.now();
  
  log_i("RTC :            %s", datetime2str(&now_rtc));

  struct tm tm_rtc;
  tm_rtc.tm_year = now_rtc.year() - 1900;
  tm_rtc.tm_mon = now_rtc.month() - 1;
  tm_rtc.tm_mday = now_rtc.day();
  tm_rtc.tm_hour = now_rtc.hour();
  tm_rtc.tm_min = now_rtc.minute();
  tm_rtc.tm_sec = now_rtc.second();

  struct timeval tv = { mktime(&tm_rtc), 0 };
  settimeofday(&tv, NULL);

  struct tm *tm_internal = get_internal_clock();
  log_i("internal clock : %s", tm2str(tm_internal));
} // ntp_adjust()

/**
 *
 */
unsigned long change_mode(unsigned long mode=MODE_N) {
  log_i("change_mode> mode=%d/%d", (int)mode, MODE_N);
  prevMode = curMode;
  if (mode < MODE_N) {
    curMode = mode;
  } else if (mode == MODE_N) {
    curMode = (curMode + 1) % MODE_N;
  } else {
    curMode = (curMode + MODE_N - 1) % MODE_N;
  }
  log_i("change_mode> curMode=%d:%s",
        (int)curMode, Mode[curMode]->name().c_str());

  nixieArray.end_all_effect();

  return curMode;
} // change_mode()

/**
 * RTC読込中に 割り込みがかかると落ちることがある
 * 設定ファイル読込中に 割り込みがかかると落ちることがある
 */
void IRAM_ATTR btn_intr_hdr() {
  static unsigned long prev_ms = 0;
  unsigned long        cur_ms = millis();

  if ( cur_ms - prev_ms < DEBOUNCE ) {
    return;
  }
  prev_ms = cur_ms;

  for (int b=0; b < BTN_N; b++) {
    if ( Btn[b]->get() ) {
      // log_i("btn_intr_hdr> ");
      // Btn[b]->print();
      
      Mode[curMode]->btn_intr_hdr(curMsec, Btn[b]);
    }
  } // for(b)
} // btn_intr_hdr()


/**
 *
 */
void btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  log_i("%s", btn->toString().c_str());

  if ( btn->get_name() == "BTN0" ) {
    if ( btn->get_click_count() >= 1 ) {
      if ( Btn[1]->get_value() == Button::ON ) {
        if ( curMode == MODE_CLOCK ) {
          change_mode(MODE_COUNT);
          return;
        }
        if ( curMode == MODE_COUNT ) {
          change_mode(MODE_CLOCK);
          return;
        }
      }
    }

    if ( btn->get_click_count() >= 3 ) {
      change_mode();
      if ( curMode == MODE_SET_CLOCK ) {
        change_mode();
      }
      return;
    }

    if ( btn->get_click_count() == 2 && curMode == MODE_CLOCK ) {
      log_i("netMgr.cur_mode=0x%02X", netMgr.cur_mode);
      wifiActive = false;
      prev_wifiActive = false;
      ntpActive = false;
      if ( netMgr.cur_mode == NetMgr::MODE_AP_LOOP
           || netMgr.cur_mode == NetMgr::MODE_WIFI_OFF ) {
        //netMgr.cur_mode = NetMgr::MODE_START;
        log_i("Reboot");
        log_i("==========");
        ESP.restart();
      } else {
        netMgr.cur_mode = NetMgr::MODE_AP_INIT;
      }
      log_i("netMgr.cur_mode=0x%02X", netMgr.cur_mode);
      delay(500);
      return;
    }
  }

  Mode[curMode]->btn_loop_hdr(cur_ms, btn);
} // btn_loop_hdr()

/**
 *
 */
void enableIntr() {
  attachInterrupt(intrPin0, btn_intr_hdr, CHANGE);
  attachInterrupt(intrPin1, btn_intr_hdr, CHANGE);
  attachInterrupt(intrPin2, btn_intr_hdr, CHANGE);
}

/**
 *
 */
void disableIntr() {
  detachInterrupt(intrPin0);
  detachInterrupt(intrPin1);
  detachInterrupt(intrPin2);
}

//=======================================================================
/**
 *
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  log_i("begin");
  Serial.println("start");
  
  for (int i=0; i < MODE_N; i++) {
    log_i("Mode[%d]:%s", i, Mode[i]->name().c_str());
  }
  
  log_i("NTP servers:");
  for (int i=0; i < 3; i++) {
    log_i(" %s", NTP_SVR[i].c_str());
  }

  //---------------------------------------------------------------------
  // グローバルオブジェクト・変数の初期化
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  NetMgr::myName = MY_NAME;

  nixieArray.brightness = BRIGHTNESS_RESOLUTION;

  ntpActive = false;

  // RTC
  log_i("RTC begin");
  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
  Rtc.begin(&Wire);
  delay(100);

  DateTime now_rtc = Rtc.now();
  log_i("RTC :            %s", datetime2str(&now_rtc));

  // adjust internal clock
  struct tm tm_rtc;
  tm_rtc.tm_year = now_rtc.year() - 1900;
  tm_rtc.tm_mon = now_rtc.month() - 1;
  tm_rtc.tm_mday = now_rtc.day();
  tm_rtc.tm_hour = now_rtc.hour();
  tm_rtc.tm_min = now_rtc.minute();
  tm_rtc.tm_sec = now_rtc.second();

  struct timeval tv = { mktime(&tm_rtc), 0 };
  settimeofday(&tv, NULL);

  struct tm *tm_internal = get_internal_clock();
  log_i("internal clock : %s", tm2str(tm_internal));
  
  randomSeed(now_rtc.second()); // TBD

  prevMsec = millis();
  curMsec = prevMsec;

  //---------------------------------------------------------------------
  // 初期状態表示
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  nixieArray.display(curMsec);

  //---------------------------------------------------------------------
  // 割り込み初期化
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  log_i("digitalPinToInterrupt:");
  log_i(" %d --> %d", PIN_BTN0, intrPin0);
  log_i(" %d --> %d", PIN_BTN1, intrPin1);
  log_i(" %d --> %d", PIN_BTN2, intrPin2);


  // !! Important !!
  // 
  // 設定ファイル読込中に 割り込みがかかると落ちることがある
  enableIntr();
} // setup()

//=======================================================================
/**
 *
 */
void loop() {
  mode_t   netmgr_mode;

  struct tm *tm_now = get_internal_clock();
  DateTime now = DateTime(tm_now->tm_year + 1900, tm_now->tm_mon + 1,
                          tm_now->tm_mday,
                          tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

  static sntp_sync_status_t prev_sntp_stat = SNTP_SYNC_STATUS_RESET;
  sntp_sync_status_t sntp_stat = sntp_get_sync_status();
  
  prevMsec = curMsec;
  curMsec = millis();
  loopCount++;

  //---------------------------------------------------------------------
  // NetMgr
  netmgr_mode = netMgr.loop();
  prev_wifiActive = wifiActive;
  if (netmgr_mode == NetMgr::MODE_WIFI_ON) {
    wifiActive = true;
    ntpActive = true;
    if ( wifiActive != prev_wifiActive ) {
      log_i("WiFi ON");
      ntp_adjust();
    }
  } else if ( netmgr_mode == NetMgr::MODE_WIFI_OFF ) {
    wifiActive = false;
    ntpActive = false;
    if ( wifiActive != prev_wifiActive ) {
      log_i("WiFi OFF");
      netMgr.cur_mode = NetMgr::MODE_START;
    }
  }
  
  //---------------------------------------------------------------------
  // NTP
  if ((curMsec - ntpLast) >= NTP_INTERVAL) {
    ntpLast = curMsec;
    ntp_adjust();
  }

  if ( prev_sntp_stat != SNTP_SYNC_STATUS_COMPLETED &&
       sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
    log_i("SNTP_SYNC_STATUS: %s --> %s",
          SNTP_SYNC_STATUS_STR[prev_sntp_stat],
          SNTP_SYNC_STATUS_STR[sntp_stat]);

    // get NTP time
    static tm tm_ntp;
    disableIntr();
    bool res_getLocalTime = getLocalTime(&tm_ntp); // NTP
    enableIntr();

    // adjust RTC
    DateTime now_rtc = DateTime(tm_ntp.tm_year + 1900,
                                tm_ntp.tm_mon + 1,
                                tm_ntp.tm_mday,
                                tm_ntp.tm_hour,
                                tm_ntp.tm_min,
                                tm_ntp.tm_sec);
    disableIntr();
    Rtc.adjust(now_rtc);
    now_rtc = Rtc.now();
    enableIntr();
    
    // adjust internal clock
    struct timeval tv = { mktime(&tm_ntp), 0 };
    settimeofday(&tv, NULL);
    struct tm *tm_internal = get_internal_clock();

    log_i("  getLocalTime()>%s : %s",
          res_getLocalTime ? "true ": "false", tm2str(&tm_ntp));
    log_i("  RTC :                  %s", datetime2str(&now_rtc));
    log_i("  internal clock :       %s", tm2str(tm_internal));
  } // if (sntp_stat)

  prev_sntp_stat = sntp_stat;

  //---------------------------------------------------------------------
  if (loopCount % 100000 == 0) {
    log_i("NTP:%s, internal clock: %s",
          SNTP_SYNC_STATUS_STR[sntp_stat],
          tm2str(tm_now));
  }

  //---------------------------------------------------------------------
  // check buttions
  for (int b=0; b < BTN_N; b++) {
    if ( ! Btn[b]->get() ) {
      continue;
    }

    btn_loop_hdr(curMsec, Btn[b]);
  } // for(b)

  //---------------------------------------------------------------------
  // モード実行
  if (curMode != prevMode) {
    // モード変更時の初期化
    Mode[curMode]->init(curMsec, now, initValVer);
    prevMode = curMode;
  } else {
    // 各モードの loop() 実行
    stat_t stat = Mode[curMode]->loop(curMsec, now);
    switch (stat) {
    case ModeBase::STAT_BACK_MODE:
      log_i("stat=0x%X, curMode=%d, prevMode=%d",
            (int)stat, (int)curMode, (int)prevMode);
      change_mode(MODE_N + 1);
      break;

    case ModeBase::STAT_NEXT_MODE:
      log_i("stat=0x%X", (int)stat);
      change_mode();
      break;

    default:
      break;
    } // switch(stat)
  }

  //---------------------------------------------------------------------
  // 表示
  nixieArray.display(curMsec);

  //---------------------------------------------------------------------
  //delayMicroseconds(LOOP_DELAY_US);
} // loop()
