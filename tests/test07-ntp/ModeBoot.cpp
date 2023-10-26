/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeBoot.h"

/**
 *
 */
ModeBoot::ModeBoot(unsigned long interval): Mode() {
  this->interval = interval;
} // ModeBoot::ModeBoot()

/** virtual
 *
 */
void ModeBoot::enter() {
  log_i("enter mode: %s", this->name.c_str());

  this->start_ms = millis();

  Nxa->set_string(VersionString.c_str());
  
  Nxa->colon[NIXIE_COLON_L].blink_start(start_ms, 200);
  delay(200);
  Nxa->colon[NIXIE_COLON_R].blink_start(start_ms+200, 200);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // ModeBoot::enter()

/** virtual
 *
 */
void ModeBoot::exit() {
  log_i("");
  Nxa->end_all_effect();
  this->start_ms = 0;
} // ModeBoot::exit()

/** virtual
 *
 */
void ModeBoot::loop() {
  unsigned long cur_ms = millis();
  struct tm *tm = SysClock::now_tm();
  struct timeval *tv = SysClock::now_timeval();

  //
  // check interval
  //
  if ( cur_ms - this->start_ms > this->interval ) {
    log_i("done");
    Flag_LoopRunning = false;
    Mode::set("ModeClock");
    return;
  }

  //
  // Disp
  //
  char *fmt_date, *fmt_time;

  fmt_date = (char *)"%Y/%m/%d(%a)";
  if ( tv->tv_sec % 2 == 0 ) {
    fmt_time =(char *)"%H:%M:%S";
  } else {
    fmt_time =(char *)"%H %M %S";
  }

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);

  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("%s\n", tm2string(tm, fmt_date).c_str());
  Disp->printf("%s\n", tm2string(tm, fmt_time).c_str());

  Disp->setCursor(0, DISPLAY_H - DISPLAY_CH_H);
  Disp->setTextSize(1);
  Disp->printf("%s", get_mac_addr_string().c_str());

  Disp->display();

  delayOrChangeMode(50);
} // ModeBoot::loop()
