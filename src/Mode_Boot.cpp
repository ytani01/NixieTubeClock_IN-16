/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Boot.h"

/**
 *
 */
Mode_Boot::Mode_Boot(unsigned long interval): Mode() {
  this->interval = interval;
} // Mode_Boot::Mode_Boot()

/** virtual
 *
 */
void Mode_Boot::enter() {
  log_i("enter mode: %s", this->name.c_str());

  this->start_ms = millis();

  Nxa->set_string("AABBCCDD");
  unsigned long ms = 80 * (BRIGHTNESS_RESOLUTION / Nxa->brightness());
  Nxa->set_string(VersionString.c_str(), NXT_EFFECT_FOG, ms);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // Mode_Boot::enter()

/** virtual
 *
 */
void Mode_Boot::exit() {
  log_i("");
  Nxa->end_all_effect();
  this->start_ms = 0;
} // Mode_Boot::exit()

/** virtual
 *
 */
void Mode_Boot::loop() {
  unsigned long cur_ms = millis();
  struct tm *tm = SysClock::now_tm();
  struct timeval *tv = SysClock::now_timeval();

  //
  // check interval
  //
  if ( cur_ms - this->start_ms > this->interval ) {
    log_i("done");
    Flag_LoopRunning = false;
    Mode::set("Mode_Clock");
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
  Disp->printf("     %s\n", tm2string(tm, fmt_time).c_str());
  Disp->printf("booting .. ");
  Mode::disp_spin(100);

  Disp->setCursor(0, DISPLAY_H - DISPLAY_CH_H);
  Disp->setTextSize(1);
  Disp->printf("%s", get_mac_addr_string().c_str());

  Disp->display();

  delayOrChangeMode(50);
} // Mode_Boot::loop()

/**
 *
 */
void Mode_Boot::cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(&bi).c_str());

  if ( String(bi.name) == "Btn0" ) {
    if ( bi.value == Button::ON ) {
      //
      // WiFi OFF (AP mode)
      //
      WiFi.disconnect();
      TaskWifiMgr->mode = WIFI_MGR_MODE_AP;
      log_i("*** force WiFi OFF (AP mode)");
      return;
    } // if (ON)

    return;
  } // if (Btn0)
  
  if ( String(bi.name) == "Btn2" ) {
    if ( bi.value == Button::ON ) {
      Mode::set("Mode_Test1");
      
      return;
    } // if (ON)

    return;
  } // if (Btn2)
  
} // Mode_Boot::cbBtn()
