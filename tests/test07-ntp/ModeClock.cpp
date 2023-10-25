/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeClock.h"

/**
 *
 */
ModeClock::ModeClock(): Mode() {
} // ModeClock::ModeClock()

/** virtual
 *
 */
void ModeClock::enter() {
  log_i("enter mode: %s", this->name.c_str());

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // ModeClock::enter()

/** virtual
 *
 */
void ModeClock::exit() {
  Nxa->end_all_effect();
} // ModeClock::exit()

/** virtual
 *
 */
void ModeClock::loop() {
  unsigned long cur_ms = millis();
  struct tm *tm = SysClock::now_tm();

  struct timeval *tv = SysClock::now_timeval();

  //
  // WiFi status
  //
  static wifi_mgr_mode_t prev_wifimgr_mode = WIFI_MGR_MODE_NONE;
  wifi_mgr_mode_t wifimgr_mode = TaskWifiMgr->mode;

  if ( wifimgr_mode != prev_wifimgr_mode ) {
    log_i("wifimgr_mode = %d:%s",
          wifimgr_mode, WIFI_MGR_MODE_T_STR[wifimgr_mode]);

    prev_wifimgr_mode = wifimgr_mode;
  }

  static wl_status_t prev_wl_stat = WL_IDLE_STATUS;
  wl_status_t wl_stat = TaskWifiMgr->wl_stat;

  if ( wl_stat != prev_wl_stat ) {
    log_i("wl_stat = %d:%s",
          wl_stat, WL_STATUS_T_STR2(wl_stat));

    if ( wl_stat == WL_CONNECTED ) {
      log_i("SSID: %s", TaskWifiMgr->cur_ssid.c_str());
    }

    prev_wl_stat = wl_stat;
  }

  //
  // for NixieTubeArray
  //
  char *nx_fmt;

  switch ( this->clock_mode ) {
  case CLOCK_MODE_HMS:
    if ( wl_stat == WL_CONNECTED ) {
      if ( tv->tv_sec % 2 == 0 ) {
        nx_fmt = ModeClock::NX_FMT_HMS1;
      } else {
        nx_fmt = ModeClock::NX_FMT_HMS2;
      }
    } else {
      if ( tv->tv_sec % 4 <= 1 ) {
        nx_fmt = ModeClock::NX_FMT_HMS1;
      } else {
        nx_fmt = ModeClock::NX_FMT_HMS2;
      }
    }
    break;

  case CLOCK_MODE_dHM:
    if ( tv->tv_sec % 2 == 0 ) {
      nx_fmt = ModeClock::NX_FMT_dHM1;
    } else {
      nx_fmt = ModeClock::NX_FMT_dHM2;
    }
    break;

  case CLOCK_MODE_ymd:
    if ( cur_ms - date_start_ms > ModeClock::CLOCK_MODE_DATE_INTERVAL ) {
      this->clock_mode = this->clock_mode_main;
      return;
    }

    nx_fmt = ModeClock::NX_FMT_ymd;
    break;

  default:
    break;
  } // switch (clock_mode)
  
  std::string nx_str = tm2string(tm, nx_fmt);
  log_v("nx_fmt = %s, nx_str = %s", nx_fmt, nx_str.c_str());

  Nxa->set_string(nx_str.c_str());

  //
  // for Disp
  //
  char *fmt_date, *fmt_time;

  fmt_date = (char *)"%Y/%m/%d(%a)";
  //if ( tv->tv_usec % 1000000 < 500000 ) {
  if ( tv->tv_sec % 2 == 0 ) {
    fmt_time =(char *)"%H:%M:%S";
  } else {
    fmt_time =(char *)"%H %M %S";
  }

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);

  //Disp->printf("%s\n",  __CLASS_NAME__.c_str());

  Disp->setTextColor(BLACK, WHITE);
  Disp->printf("%s\n", tm2string(tm, fmt_date).c_str());
  Disp->setTextColor(WHITE, BLACK);

  Disp->setTextSize(1);
  Disp->printf("%s\n", tm2string(tm, fmt_time).c_str());

  if ( wifimgr_mode == WIFI_MGR_MODE_STA ) {
    if ( wl_stat == WL_CONNECTED ) {
      Disp->printf("%s", TaskWifiMgr->cur_ssid.c_str());
    } else {
      if ( cur_ms % 1000 > 500 ) {
        Disp->printf(">%s\n", WL_STATUS_T_STR2(wl_stat));
      } else {
        Disp->printf("\n");
      }
    }
  } else {
    if ( cur_ms % 2000 > 200 ) {
      Disp->printf("AP:%s\n", TaskWifiMgr->ap_ssid.c_str());
    } else {
      Disp->printf("\n");
    }
  }
  
  Disp->setCursor(0, DISPLAY_H - DISPLAY_CH_H);
  Disp->setTextSize(1);
  Disp->printf("%s", get_mac_addr_string().c_str());

  Disp->display();

  delayOrChangeMode(50);
} // ModeClock::loop()

/**
 *
 */
void ModeClock::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( bi->long_pressed ) {
        if ( bi->repeat_count == 0 ) {
          Mode::set("ModeB");
        }
      }
    }
    return;
  } // if (Btn0)

  if ( String(bi->name) == "Btn1" ) {
    if ( bi->value == Button::ON ) {
      if ( bi->push_count == 1 ) {
        if ( ! bi->long_pressed ) {
          // simply push
          if ( this->clock_mode != CLOCK_MODE_ymd ) {
            this->clock_mode = CLOCK_MODE_ymd;
            this->date_start_ms = millis();
          } else {
            this->clock_mode = this->clock_mode_main;
          }
        } else { // long_pressed
          // long pressed
          if ( bi->repeat_count == 1 ) {
            if ( this->clock_mode_main == CLOCK_MODE_HMS ) {
              this->clock_mode_main = CLOCK_MODE_dHM;
            } else {
              this->clock_mode_main = CLOCK_MODE_HMS;
            }
            this->clock_mode = this->clock_mode_main;
            this->date_start_ms = 0;
          }
        } // if (!long_pressed)
      } // if (push_count==1)
    } // if (ON)
    
    log_v("clock_mode_main = %d, clock_mode = %d",
          this->clock_mode_main, this->clock_mode);
    return;
  } // if (Btn1)

  if ( String(bi->name) == "Btn2" ) {
    if ( bi->value == Button::ON ) {
      brightness_t bri = Nxa->brightness() / 2;
      if ( bri < BRIGHTNESS_MIN ) {
        bri = BRIGHTNESS_RESOLUTION;
      }
      Nxa->set_brightness(bri);
    }
    return;
  } // if (Btn2)

} // ModeClock::cbBtn()
