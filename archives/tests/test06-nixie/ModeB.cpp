/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeB.h"

/** virtual
 *
 */
void ModeB::enter() {
  log_i("enter mode: %s", this->name.c_str());

  Nxa->num[1].blink_start(millis(), 500);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setTextWrap(true);
  Disp->setCursor(0, 0);

  Disp->printf("%s", this->name.c_str());

  Disp->display();
} // ModeB::enter()

/** virtual
 *
 */
void ModeB::exit() {
  Nxa->end_all_effect();
} // ModeB::exit()

/** virtual
 *
 */
void ModeB::loop() {
  struct tm *tm = SysClock::now_tm();
  char *fmt_date, *fmt_time;

  fmt_date = (char *)"%Y/%m/%d(%a)";
  if ( (millis() % 1000 / 500) == 0 ) {
    fmt_time = (char *)"%H:%M:%S";
  } else {
    fmt_time = (char *)"%H %M %S";
  }

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);

  Disp->printf("%s\n",  __CLASS_NAME__.c_str());
  Disp->printf("%s\n",  tm2string(tm, fmt_date).c_str());

  Disp->setTextSize(2);
  Disp->setTextColor(BLACK, WHITE);

  Disp->printf("%s",  tm2string(tm, fmt_time).c_str());

  Disp->setTextColor(WHITE, BLACK);

  Disp->display();

  delayOrChangeMode(100);
} // ModeB::loop()

/**
 *
 */
void ModeB::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  struct tm *tm_sys = SysClock::now_tm();

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( ! bi->long_pressed ) {
        // reset second
        tm_sys->tm_sec = 0;

        // adjust SysClock
        SysClock::set(tm_sys);
        log_d("Sys : %s", tm2string(SysClock::now_tm()).c_str());
        
        // adjust RTC
        Rtc->adjust(tm_sys);
        DateTime dt = Rtc->now();
        log_d("RTC : %s", datetime2string(&dt).c_str());

        Mode::set("ModeA");
      }
    }
    return;
  } // if (Btn0)

  bool flag_set_clock = false;

  if ( String(bi->name) == "Btn1" ) {
    if ( bi->value == Button::ON || bi->long_pressed ) {
      if ( bi->repeat_count < 9 ) {
        tm_sys->tm_min++;
      } else {
        tm_sys->tm_hour++;
      }
      flag_set_clock = true;
    }
  } // if (Btn1)

  if ( String(bi->name) == "Btn2" ) {
    if ( bi->value == Button::ON || bi->long_pressed ) {
      if ( bi->repeat_count < 9 ) {
        tm_sys->tm_min--;
      } else {
        tm_sys->tm_hour--;
      }
      flag_set_clock = true;
    }
  } // if (Btn2)

  if ( flag_set_clock ) {
    // adjust SysClock
    SysClock::set(tm_sys);
    log_d("Sys : %s", tm2string(SysClock::now_tm()).c_str());
    
    // adjust RTC
    Rtc->adjust(tm_sys);
    DateTime dt = Rtc->now();
    log_d("RTC : %s", datetime2string(&dt).c_str());
  }
} // ModeB::cbBtn()
