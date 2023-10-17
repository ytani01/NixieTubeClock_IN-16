/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeA.h"

/**
 *
 */
ModeA::ModeA(): Mode() {
} // ModeA::ModeA()

/**
 *
 */
void ModeA::loop() {
  log_d("%s: class %s: name: %s",
        SysClock::now_str(),
        __CLASS_NAME__.c_str(), this->name.c_str());
  delay(5000);
} // ModeA::loop()

/**
 *
 */
void ModeA::cbBtn(ButtonInfo_t *bi) {
  struct tm *tm_sys = SysClock::now_tm();

  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( ! bi->long_pressed ) {
        Mode::set("ModeB");
      }
    }
    return;
  } // if (Btn0)

  if ( String(bi->name) == "Btn1" ) {
    if ( bi->value == Button::ON || bi->long_pressed ) {
      if ( bi->repeat_count < 9 ) {
        tm_sys->tm_mday++;
        tm_sys->tm_wday++;
      } else if ( bi->repeat_count < 19 ) {
        tm_sys->tm_mon++;
      } else {
        tm_sys->tm_year++;
      }
      log_d(">tm_sys: %s", tm2str(tm_sys));

      SysClock::set(tm_sys);

      // adjust RTC
      disableIntr();
      
      Rtc->adjust_tm(tm_sys);
      DateTime dt_rtc = Rtc->now();
      
      enableIntr();
      
      log_d(">dt_rtc: %s", datetime2str(&dt_rtc));
    }
    return;
  } // if (Btn1)

  if ( String(bi->name) == "Btn2" ) {
    if ( bi->value == Button::ON || bi->long_pressed ) {
      if ( bi->repeat_count < 9 ) {
        tm_sys->tm_mday--;
        tm_sys->tm_wday--;
      } else if ( bi->repeat_count < 19 ) {
        tm_sys->tm_mon--;
      } else {
        tm_sys->tm_year--;
      }
      log_d(">tm_sys: %s", tm2str(tm_sys));

      SysClock::set(tm_sys);

      // adjust RTC
      disableIntr();
      
      Rtc->adjust_tm(tm_sys);
      DateTime dt_rtc = Rtc->now();
      
      enableIntr();
      
      log_d(">dt_rtc: %s", datetime2str(&dt_rtc));
    }
    return;
  } // if (Btn2)

} // ModeA::cbBtn()
