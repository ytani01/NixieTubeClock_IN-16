/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <map>
#include <esp32-hal-log.h>

#include "SysClock.h"
#include "MyRtc.h"
#include "Display.h"

#include "Task_ButtonWatcher.h"
#include "Task_NixieTubeArray.h"

#include "Mode.h"
#include "ModeA.h"
#include "ModeB.h"

#include "ConfFile_Ssid.h"
ConfFile_Ssid *ConfFileSsid;


bool Flag_ReqModeChange = false;
bool Flag_LoopRunning = false;

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
Task_ButtonWatcher *taskBtnWatcher = NULL;

// Nixie Tube
#define PIN_HV5812_CLK      7
#define PIN_HV5812_STOBE    5
#define PIN_HV5812_DATA     6
#define PIN_HV5812_BLANK    4

#define PIN_COLON_R_TOP     2
#define PIN_COLON_R_BOTTOM  2
#define PIN_COLON_L_TOP     1
#define PIN_COLON_L_BOTTOM  1

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

NixieTubeArray *nta = NULL;

Task_NixieTubeArray *taskNixieTubeArray = NULL;

/** global
 *
 */
void enableIntr() {
  if ( taskBtnWatcher ) {
    taskBtnWatcher->enable();
  }
}

/** global
 *
 */
void disableIntr() {
  if ( taskBtnWatcher ) {
    taskBtnWatcher->disable();
  }
}

/**
 *
 */
void cbBtn(ButtonInfo_t *bi) {
  Mode::Cur->cbBtn(bi);
} // cbBtn()

/**
 *
 */
void setup() {
  delay(2000);
  log_i("===== start %s SysClock: %s =====",
        get_mac_addr_String().c_str(),
        SysClock::now_string().c_str());

  // I2C
  log_i("=== Init I2C: SDA=%d, SCL=%d", PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
  
  // RTC
  log_i("=== Init RTC");
  Rtc = new MyRtc();
  Rtc->begin(&Wire);
  DateTime dt_rtc = Rtc->now();
  log_i("RTC      : %s", datetime2string(&dt_rtc).c_str());

  // Display
  log_i("=== Init Display");
  Disp = new Display_t(DISPLAY_W, DISPLAY_H);
  Disp->DispBegin(DISP_ADDR);
  Disp->setRotation(0);
  //Disp->clearDisplay();
  Disp->display();

  // SysClock
  log_i("=== Init System Clock");
  SysClock::set(&dt_rtc);
  log_i("SysClock : %s", SysClock::now_string().c_str());

  // Button
  log_i("=== Init Buttons");
  taskBtnWatcher = new Task_ButtonWatcher(cbBtn);

  for (auto btn: PIN_BTN) {
    log_i("{%s %d}", btn.first, btn.second);
    taskBtnWatcher->addBtn(String(btn.first), btn.second);
  }

  taskBtnWatcher->start();
  delay(100);

  // NixieTube
  log_i("=== Nixie Tube Array");
  nta = new NixieTubeArray(PIN_HV5812_CLK,  PIN_HV5812_STOBE,
                               PIN_HV5812_DATA, PIN_HV5812_BLANK,
                               PINS_NIXIE_NUM, PINS_NIXIE_COLON);

  taskNixieTubeArray = new Task_NixieTubeArray(nta);
  taskNixieTubeArray->start();

  nta->num[0].blink_start(millis(), 500);

  // Mode
  log_i("=== Init Modes");
  Mode::add("ModeA", new ModeA());
  Mode::add("ModeB", new ModeB());

  for (auto m: Mode::Ent) {
    m.second->setup();
  }

  Mode::set("ModeA");

  //
  // XXX: Conf test
  //
  ConfFileSsid = new ConfFile_Ssid();

  int conf_n;
  if ( conf_n = ConfFileSsid->load() > 0 ) {
    for (auto ent: ConfFileSsid->ent) {
      log_d("%s:%s", ent.first.c_str(), ent.second.c_str());
    }
  }

  //ConfFileSsid->remove();
  
  ConfFileSsid->ent["aaa"] = "AAA";
  ConfFileSsid->ent["bbb"] = "DDD";

  ConfFileSsid->ent.erase("aaa");
  conf_n = ConfFileSsid->save();
  log_d("conf_n = %d", conf_n);

  if ( conf_n = ConfFileSsid->load() > 0 ) {
    for (auto ent: ConfFileSsid->ent) {
      log_d("%s:%s", ent.first.c_str(), ent.second.c_str());
    }
  }

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
  log_v("class %s", __CLASS_NAME__.c_str());

  if ( Mode::Cur ) {
    Flag_LoopRunning = true;
    Mode::Cur->loop();
    if ( Flag_ReqModeChange ) {
      log_d("%s::loop(): done", Mode::Prev->name.c_str());
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
      log_v("wait_count=%u", wait_count);
    }
  } else {
    log_e("Mode::Cur == NULL !?");
    delayOrChangeMode(2000);
  }
} // loop()
