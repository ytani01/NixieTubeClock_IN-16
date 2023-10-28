/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include <map>
#include <esp32-hal-log.h>

#include "Task_ButtonWatcher.h"
#include "SysClock.h"
#include "MyRtc.h"

#include "Mode.h"
#include "ModeA.h"
#include "ModeB.h"

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
    log_i(" tm_sys: %s", tm2str(tm_sys));

    if (String(bi->name) == "Btn0") {
      DateTime dt_sys = DateTime(tm_sys->tm_year + 1900,
                                 tm_sys->tm_mon + 1,
                                 tm_sys->tm_mday,
                                 tm_sys->tm_hour,
                                 tm_sys->tm_min,
                                 tm_sys->tm_sec);
      
      taskBtnWatcher->disable();
      Rtc->adjust(dt_sys);
      DateTime dt_rtc = Rtc->now();
      log_i(" dt_rtc: %s", datetime2str(&dt_rtc));
      taskBtnWatcher->enable();

      if ( ! Mode::Cur ) {
        log_w("Mode::Cur is NULL !?");
        return;
      }
      if ( Mode::Cur->name == "ModeA" ) {
        Mode::set("ModeB");
        return;
      }
      if ( Mode::Cur->name == "ModeB" ) {
        Mode::set("ModeA");
        return;
      }
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

  // RTC
  Rtc = new MyRtc();
  Rtc->begin();
  DateTime dt_rtc = Rtc->now();
  log_i("RTC      : %s", datetime2str(&dt_rtc));

  // SysClock
  SysClock::set(&dt_rtc);
  log_i("SysClock : %s", SysClock::now_str());

  // Button
  taskBtnWatcher = new Task_ButtonWatcher(cbBtn);

  for (auto btn: PIN_BTN) {
    log_v("{%s %d}", btn.first, btn.second);
    taskBtnWatcher->addBtn(String(btn.first), btn.second);
  }

  taskBtnWatcher->start();

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
void loop() {
  log_v("class %s", __CLASS_NAME__.c_str());

  if ( Mode::Cur ) {
    Mode::Cur->loop();
  } else {
    log_e("Mode::Cur == NULL !?");
    delay(2000);
  }
} // loop()
