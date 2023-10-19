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
bool ModeA::enter() {
  log_d("enter mode: %s", this->name.c_str());

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();

  return true;
} // ModeA::en

/**
 *
 */
void ModeA::loop() {
  struct tm *tm = SysClock::now_tm();
  char *fmt_date, *fmt_time;

  fmt_date = (char *)"%Y/%m/%d(%a)";
  if ( (millis() % 1000 / 500) == 0 ) {
    fmt_time =(char *)"%H:%M:%S";
  } else {
    fmt_time =(char *)"%H %M %S";
  }

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);

  Disp->printf("%s\n",  __CLASS_NAME__.c_str());

  Disp->setTextColor(BLACK, WHITE);
  Disp->printf("%s\n", tm2string(tm, fmt_date).c_str());
  Disp->setTextColor(WHITE, BLACK);

  Disp->setTextSize(2);
  Disp->printf("%s", tm2string(tm, fmt_time).c_str());

  Disp->display();

  delayOrChangeMode(100);
} // ModeA::loop()

/**
 *
 */
void ModeA::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( ! bi->long_pressed ) {
        Mode::set("ModeB");
      }
    }
    return;
  } // if (Btn0)

  bool flag_set_clock = false;
  struct tm *tm_sys = SysClock::now_tm();  

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
      flag_set_clock = true;
    }
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
} // ModeA::cbBtn()
