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
  std::string fmt;

  if ( millis() % 1000 < 500 ) {
    fmt ="%Y/%m/%d(%a)\n%H:%M:%S";
  } else {
    fmt ="%Y/%m/%d(%a)\n%H %M %S";
  }

  char *tm_str = tm2str(tm, fmt.c_str());

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->printf("%s\n%s",  __CLASS_NAME__.c_str(), tm_str);
  Disp->display();

  unsigned long delay_ms = delayOrChangeMode(5000);
  log_d("%s: %u ms", __CLASS_NAME__.c_str(), delay_ms);
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
    log_d(">tm_sys: %s", tm2str(tm_sys));

    SysClock::set(tm_sys);

    // adjust RTC
    disableIntr();
    
    Rtc->adjust(tm_sys);
    DateTime dt_rtc = Rtc->now();
    
    enableIntr();
    
    log_d(">dt_rtc: %s", datetime2str(&dt_rtc));
  }
} // ModeA::cbBtn()
