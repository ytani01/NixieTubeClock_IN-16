/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeReboot.h"

/**
 *
 */
ModeReboot::ModeReboot(unsigned long interval): Mode() {
  this->interval = interval;
} // ModeReboot::ModeReboot()

/** virtual
 *
 */
void ModeReboot::enter() {
  log_i("enter mode: %s", this->name.c_str());

  this->start_ms = millis();

  Nxa->set_string(VersionString.c_str());
  unsigned long ms = 80 * (BRIGHTNESS_RESOLUTION / Nxa->brightness());
  Nxa->set_string("        ", NXA_EFFECT_XFADE, ms);
#if 0
  Nxa->num[5].effect_blink(300, start_ms);
  Nxa->num[4].effect_blink(300, start_ms);
  Nxa->colon[NIXIE_COLON_R].effect_blink(300, this->start_ms+20);
  Nxa->num[3].effect_blink(300, this->start_ms+40);
  Nxa->num[2].effect_blink(300, this->start_ms+40);
  Nxa->colon[NIXIE_COLON_L].effect_blink(300, this->start_ms+60);
  Nxa->num[1].effect_blink(300, this->start_ms+80);
  Nxa->num[0].effect_blink(300, this->start_ms+80);
#endif
  
  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->display();
} // ModeReboot::enter()

/** virtual
 *
 */
void ModeReboot::exit() {
  log_i("");
  Nxa->end_all_effect();
  this->start_ms = 0;
} // ModeReboot::exit()

/** virtual
 *
 */
void ModeReboot::loop() {
  unsigned long cur_ms = millis();
  struct tm *tm = SysClock::now_tm();
  struct timeval *tv = SysClock::now_timeval();

  //
  // check interval
  //
  if ( cur_ms - this->start_ms > this->interval ) {
    log_i("done");
    Flag_LoopRunning = false;

    ESP.restart();
    return;
  }

  //
  // Disp
  //
  char *fmt_date, *fmt_time;

  fmt_date = (char *)"%Y/%m/%d(%a)";
  if ( tm->tm_sec % 2 == 0 ) {
    fmt_time =(char *)"%H:%M:%S";
  } else {
    fmt_time =(char *)"%H %M %S";
  }

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);

  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("%s\n", tm2string(tm, fmt_date).c_str());
  Disp->printf("     %s\n", tm2string(tm, fmt_time).c_str());
  Disp->printf("reboot .. ");
  Mode::disp_spin(100);

  Disp->setCursor(0, DISPLAY_H - DISPLAY_CH_H);
  Disp->setTextSize(1);
  Disp->printf("%s", get_mac_addr_string().c_str());

  Disp->display();

  delayOrChangeMode(50);
} // ModeReboot::loop()

/**
 *
 */
void ModeReboot::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      WiFi.disconnect();
      TaskWifiMgr->mode = WIFI_MGR_MODE_AP;
    }
  }
} // ModeReboot::cbBtn()
