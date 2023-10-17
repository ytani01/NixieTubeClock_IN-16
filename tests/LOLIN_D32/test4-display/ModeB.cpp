/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeB.h"

/**
 *
 */
bool ModeB::enter() {
  log_d("enter mode: %s", this->name.c_str());

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, WHITE);
  Disp->setTextColor(BLACK, WHITE);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();

  return true;
} // ModeB::enter()

/**
 *
 */
void ModeB::loop() {
  log_d("%s: name: %s",
        SysClock::now_str(),
        this->name.c_str());
  delay(5000);
} // ModeB::loop()

/**
 *
 */
void ModeB::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( ! bi->long_pressed ) {
        Mode::set("ModeA");
      }
    }
    return;
  } // if (Btn0)

  bool flag_set_clock = false;
  struct tm *tm_sys = SysClock::now_tm();

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
    log_d(">tm_sys: %s", tm2str(tm_sys));

    SysClock::set(tm_sys);
    
    // adjust RTC
    disableIntr();
    
    Rtc->adjust(tm_sys);
    DateTime dt_rtc = Rtc->now();
    
    enableIntr();
    
    log_d(">dt_rtc: %s", datetime2str(&dt_rtc));
  }
} // ModeB::cbBtn()
