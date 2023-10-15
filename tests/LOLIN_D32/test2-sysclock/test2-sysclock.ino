/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <map>
#include <esp32-hal-log.h>
#include "Task_ButtonWatcher.h"
#include "SysClock.h"
#include "MyRtc.h"

// Buttons
static const std::map<const char *, uint8_t> PIN_BTN = {
  {"Btn0", 14},
  {"Btn1", 17},
  {"Btn2", 18},
};
Task_ButtonWatcher *taskBtnWatcher = NULL;

// RTC
MyRtc *Rtc;

/**
 *
 */
void cbBtn(ButtonInfo_t *bi) {
  if ( bi->click_count >= 1 ) {
    log_d("%s", Button::info2String(bi).c_str());
    return;
  }
  if ( bi->value == Button::ON ) {
    log_i("%s", Button::info2String(bi).c_str());

    struct tm *tm_sys = SysClock::now_tm();
    log_i("tm_sys: %s", tm2str(tm_sys));

    if (String(bi->name) == "Btn0") {
      DateTime dt_sys = DateTime(tm_sys->tm_year + 1900,
                                 tm_sys->tm_mon + 1,
                                 tm_sys->tm_mday,
                                 tm_sys->tm_hour,
                                 tm_sys->tm_min,
                                 tm_sys->tm_sec);
      Rtc->adjust(dt_sys);
      DateTime dt_rtc = Rtc->now();
      log_i("dt_rtc=%s", datetime2str(&dt_rtc));
    }

    if (String(bi->name) == "Btn1") {
      tm_sys->tm_mon++;
      log_i(">tm_sys: %s", tm2str(tm_sys));
    }

    if (String(bi->name) == "Btn2") {
      tm_sys->tm_mon--;
      log_i(">tm_sys: %s", tm2str(tm_sys));
    }

    SysClock::set(tm_sys);
    return;
  }
} // cbBtn()

/**
 *
 */
void setup() {
  //Serial.begin(115200);
  delay(2000); // 2000 ms 以上が安全
  //Serial.println("start");
  log_i("===== start %s =====", get_mac_addr_String().c_str());

  log_i("SysClock : %s", SysClock::now_str());

  Rtc = new MyRtc();
  Rtc->begin();
  DateTime dt_rtc = Rtc->now();
  log_i("RTC      : %s", datetime2str(&dt_rtc));

  SysClock::set(&dt_rtc);
  log_i("SysClock : %s", SysClock::now_str());

  taskBtnWatcher = new Task_ButtonWatcher(cbBtn);
  for (auto btn: PIN_BTN) {
    log_i("{%s %d}", btn.first, btn.second);
    taskBtnWatcher->addBtn(String(btn.first), btn.second);
  }
  taskBtnWatcher->start();

} // setup()

/**
 *
 */
void loop() {
  log_i("%s, enable", SysClock::now_str());
  taskBtnWatcher->enable();
  delay(5000);

  log_i("%s, disable", SysClock::now_str());
  taskBtnWatcher->disable();
  delay(2000);
} // loop()
