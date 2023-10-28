/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <map>
#include <esp32-hal-log.h>

#include "Task_ButtonWatcher.h"
#include "SysClock.h"
#include "MyRtc.h"
#include "Display.h"

#include "Mode.h"
#include "ModeA.h"
#include "ModeB.h"


bool Flag_ReqModeChange = false;
bool Flag_LoopRunning = false;

// OLED Display
Display_t *Disp;

// Buttons
static const std::map<const char *, uint8_t> PIN_BTN = {
  {"Btn0", 14},
  {"Btn1", 17},
  {"Btn2", 18},
};
Task_ButtonWatcher *taskBtnWatcher = NULL;

// RTC
MyRtc *Rtc;


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

  // Display
  Disp = new Display_t(DISPLAY_W, DISPLAY_H);
  Disp->DispBegin(DISP_ADDR);
  Disp->setRotation(0);
  //Disp->clearDisplay();
  Disp->display();

  // RTC
  Rtc = new MyRtc();
  Rtc->begin();
  DateTime dt_rtc = Rtc->now();
  log_i("RTC      : %s", datetime2string(&dt_rtc).c_str());

  // SysClock
  SysClock::set(&dt_rtc);
  log_i("SysClock : %s", SysClock::now_string().c_str());

  // Button
  taskBtnWatcher = new Task_ButtonWatcher(cbBtn);

  for (auto btn: PIN_BTN) {
    log_v("{%s %d}", btn.first, btn.second);
    taskBtnWatcher->addBtn(String(btn.first), btn.second);
  }

  taskBtnWatcher->start();
  delay(100);

  // Mode
  Mode::add("ModeA", new ModeA());
  Mode::add("ModeB", new ModeB());

  for (auto m: Mode::Ent) {
    m.second->setup();
  }

  Mode::set("ModeA");

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
