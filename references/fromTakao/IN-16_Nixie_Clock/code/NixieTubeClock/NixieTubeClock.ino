/**
 * Copyright (c) 2021 Yoichi Tanibayashi
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
#include <Adafruit_NeoPixel.h>
#include "PCF8563.h"
#include "Wire.h"

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
int                 initValVer[NIXIE_NUM_N] = {0,0, 9,2, 0,0};

#define LOOP_DELAY_US   1   // micro sbeconds
#define DEBOUNCE        300 // msec

String dayOfWeekStr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

//======================================================================
// pins
//----------------------------------------------------------------------
#define PIN_HV5812_CLK     7
#define PIN_HV5812_STOBE   5
#define PIN_HV5812_DATA    6
#define PIN_HV5812_BLANK    4

#define PIN_COLON_R_TOP    2
#define PIN_COLON_R_BOTTOM 2
#define PIN_COLON_L_TOP    1
#define PIN_COLON_L_BOTTOM 1

#define PIN_BTN0           14
#define PIN_BTN1           17
#define PIN_BTN2           18

#define PIN_PIXEL          27

#define PIN_I2C_SDA        9
#define PIN_I2C_SCL        8

uint8_t pinsIn[] = {PIN_BTN0, PIN_BTN1, PIN_BTN2};

uint8_t nixiePins[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N] =
  {
   {9, 0, 6, 2, 3, 4, 5, 1, 7, 8},
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

//======================================================================
// RTC DS3231
//----------------------------------------------------------------------
// RTC_DS3231 Rtc;
TwoWire wire = TwoWire(0);
PCF8563 Rtc;


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
// NeoPixel
//----------------------------------------------------------------------
const uint8_t PIXEL_N  = 6;
const int     PIXEL_BL = 255;
static const unsigned long PixelCol[] =
  {0x000000, 0xff0000, 0x00ff00, 0xff00ff, 0x0000ff};
   
// Adafruit_NeoPixel Pixels(PIXEL_N, PIN_PIXEL, NEO_GRB + NEO_KHZ800);

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
unsigned long loopCount = 0;
unsigned long curMsec   = 0; // msec
unsigned long prevMsec  = 0; // msec

//======================================================================
/**
 *
 */
void ntp_adjust() {
  struct tm time_info;

  if (ntpActive) {
    Serial.printf("ntp_adjust> ntpActive=true\n");
  } else {
    Serial.printf("ntp_adjust> ntpActive=false\n");
    return;
  }
  
  disableIntr();
  getLocalTime(&time_info); // NTP
  Serial.printf("ntp_adjust> %04d/%02d/%02d(%s) %02d:%02d:%02d",
                time_info.tm_year + 1900,
                time_info.tm_mon + 1,
                time_info.tm_mday,
                dayOfWeekStr[time_info.tm_wday].c_str(),
                time_info.tm_hour,
                time_info.tm_min,
                time_info.tm_sec);

  if ( time_info.tm_year + 1900 > 2000 ) {
    Serial.println();
    DateTime now = DateTime(time_info.tm_year + 1900,
                            time_info.tm_mon + 1,
                            time_info.tm_mday,
                            time_info.tm_hour,
                            time_info.tm_min,
                            time_info.tm_sec);
    Rtc.adjust(now);
  } else {
    Serial.printf(" .. ignored !\n");
  }
  enableIntr();
} // ntp_adjust()

/**
 *
 */
unsigned long change_mode(unsigned long mode=MODE_N) {
  // Serial.printf("change_mode> mode=%d/%d\n", (int)mode, MODE_N);
  prevMode = curMode;
  if (mode < MODE_N) {
    curMode = mode;
  } else if (mode == MODE_N) {
    curMode = (curMode + 1) % MODE_N;
  } else {
    curMode = (curMode + MODE_N - 1) % MODE_N;
  }
  //Serial.printf("change_mode> curMode=%d:%s\n",
  //                (int)curMode, Mode[curMode]->name().c_str());

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
      // Serial.print("btn_intr_hdr> ");
      // Btn[b]->print();
      
      Mode[curMode]->btn_intr_hdr(curMsec, Btn[b]);
    }
  } // for(b)
}

/**
 *
 */
void btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  Serial.print("btn_loop_hdr> ");
  btn->print();

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
      Serial.printf("btn_loop_hdr> netMgr.cur_mode=0x%02X\n", netMgr.cur_mode);
      wifiActive = false;
      prev_wifiActive = false;
      ntpActive = false;
      if ( netMgr.cur_mode == NetMgr::MODE_AP_LOOP
           || netMgr.cur_mode == NetMgr::MODE_WIFI_OFF ) {
        //netMgr.cur_mode = NetMgr::MODE_START;
        Serial.println("btn_loop_hdr> Reboot");
        Serial.println("==========");
        Serial.println();
        ESP.restart();
      } else {
        netMgr.cur_mode = NetMgr::MODE_AP_INIT;
      }
      Serial.printf("btn_loop_hdr> netMgr.cur_mode=0x%02X\n", netMgr.cur_mode);
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
  Serial.println("setup> begin");

  for (int i=0; i < MODE_N; i++) {
    Serial.printf("Mode[%d]:%s\n", i, Mode[i]->name().c_str());
  }
  
  Serial.printf("NTP servers:");
  for (int i=0; i < 3; i++) {
    Serial.printf(" %s", NTP_SVR[i].c_str());
  }
  Serial.println();



  //---------------------------------------------------------------------
  // NeoPixel
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Pixels.begin();
  // Pixels.clear();

  //---------------------------------------------------------------------
  // グローバルオブジェクト・変数の初期化
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  NetMgr::myName = MY_NAME;
  nixieArray.brightness = BRIGHTNESS_RESOLUTION;
  ntpActive = false;

  Serial.println("setup> RTC begin");
  wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Rtc.begin(&wire);
  modeSetClock.attachI2C(&wire);

  unsigned long sec = Rtc.now().second();
  randomSeed(sec); // TBD

  prevMsec = millis();
  curMsec = prevMsec;
  //---------------------------------------------------------------------
  // 初期状態表示
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  nixieArray.display(curMsec);
  //---------------------------------------------------------------------
  // 割り込み初期化
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Serial.println("digitalPinToInterrupt:");
  Serial.printf(" %d --> %d\n", PIN_BTN0, intrPin0);
  Serial.printf(" %d --> %d\n", PIN_BTN1, intrPin1);
  Serial.printf(" %d --> %d\n", PIN_BTN2, intrPin2);


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

  disableIntr();
  DateTime now = Rtc.now();
  enableIntr();

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
      Serial.println("loop> WiFi ON");
      configTime(9 * 3600L, 0,
                 NTP_SVR[0].c_str(),
                 NTP_SVR[1].c_str(),
                 NTP_SVR[2].c_str());
      ntp_adjust();
    }
  } else if ( netmgr_mode == NetMgr::MODE_WIFI_OFF ) {
    wifiActive = false;
    ntpActive = false;
    if ( wifiActive != prev_wifiActive ) {
      Serial.println("loop> WiFi OFF");
      netMgr.cur_mode = NetMgr::MODE_START;
    }
  }
  
  //---------------------------------------------------------------------
  // NTP
  if ((curMsec - ntpLast) >= NTP_INTERVAL) {
    ntpLast = curMsec;
    ntp_adjust();
  }

  //---------------------------------------------------------------------
  if (loopCount % 3000 == 0) {
    Serial.printf("loop> now=%04d/%02d/%02d(%s) %02d:%02d:%02d",
                  now.year(), now.month(), now.day(),
                  dayOfWeekStr[now.dayOfTheWeek()].c_str(),
                  now.hour(), now.minute(), now.second());
    Serial.printf(", brightness=%d", nixieArray.brightness);
    Serial.println();
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
    // for (int i=0; i < PIXEL_N; i++) {
    //   Pixels.setPixelColor(i, PixelCol[curMode]);
    // }
    // Pixels.show();
  } else {
    // 各モードの loop() 実行
    stat_t stat = Mode[curMode]->loop(curMsec, now);
    switch (stat) {
    case ModeBase::STAT_BACK_MODE:
      Serial.printf("loop> stat=0x%X, curMode=%d, prevMode=%d\n",
                    (int)stat, (int)curMode, (int)prevMode);
      change_mode(MODE_N + 1);
      break;

    case ModeBase::STAT_NEXT_MODE:
      Serial.printf("loop> stat=0x%X\n", (int)stat);
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
  delayMicroseconds(LOOP_DELAY_US);
} // loop()
