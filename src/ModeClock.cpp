/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeClock.h"

/**
 *
 */
ModeClock::ModeClock(): Mode() {
  //
  // load config file
  //
  this->conf = new ConfFile_ModeClock();
  this->conf->load();

  if ( this->conf->clock_mode >= CLOCK_MODE_SIZE ) {
    this->conf->clock_mode = CLOCK_MODE_HMS;
    this->conf->save();
  }
  if ( this->conf->effect_i >= this->effect.size() ) {
    this->conf->effect_i = 0;
    this->conf->save();
  }
  
  this->clock_mode_main = (clock_mode_t)this->conf->clock_mode;
  this->clock_mode = this->clock_mode_main;

} // ModeClock::ModeClock()

/** virtual
 *
 */
void ModeClock::enter() {
  log_i("enter mode: %s", this->name.c_str());

  Nxa->set_string("        ");

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();

  delay(10);
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
  // for Disp
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

  Disp->printf("%s", tm2string(tm, fmt_date).c_str());

  if ( wifimgr_mode == WIFI_MGR_MODE_AP ) {
    Disp->printf("  RTC");
  } else {
    if ( TaskNtp->info.sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
      Disp->printf("  NTP");
    } else {
      if ( wl_stat == WL_CONNECTED ) {
        if ( millis() % 500 >= 250 ) {
          Disp->printf("  NTP");
        }
      } else {
        if ( millis() % 1500 >= 750 ) {
          Disp->printf("  NTP");
        }
      }
    }
  }
  Disp->printf("\n");
  
  Disp->printf(" %s    %7u\n",
               tm2string(tm, fmt_time).c_str(),
               esp_get_free_heap_size());

  if ( wifimgr_mode == WIFI_MGR_MODE_STA ) {
    if ( wl_stat == WL_CONNECTED ) {
      Disp->printf("%s\n", TaskWifiMgr->cur_ssid.c_str());
      Disp->printf("%s\n", WiFi.localIP().toString().c_str());
    } else {
      Mode::disp_spin(100);
      if ( cur_ms % 1000 > 500 ) {
        static wl_status_t prev_wl_stat = WL_IDLE_STATUS;
        static WiFiEvent_t prev_evid = (WiFiEvent_t)NULL;
        static char *str = (char *)"";
        if ( wl_stat != prev_wl_stat ) {
          str = (char *)WL_STATUS_T_STR2(wl_stat);
          prev_wl_stat = wl_stat;
        }
        if ( Task_WifiMgr::LastEvId != prev_evid ) {
          str = Task_WifiMgr::LastEvStr;
          prev_evid = Task_WifiMgr::LastEvId;
        }
        Disp->printf("%s\n", str);
      } else {
        Disp->printf("\n");
      }
    }
  } else { // WIFI_MGR_MODE_AP
    if ( cur_ms % 5000 >= 0 ) {
      Disp->printf("<<WiFi Setup SSID>>\n%s\n", TaskWifiMgr->ap_ssid.c_str());
    } else {
      Disp->printf("\n\n");
    }
  }
  
  Disp->printf("%s", get_mac_addr_string().c_str());

  Disp->display();

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
      //if ( tv->tv_sec % 4 <= 2 ) {
      if ( tv->tv_sec % 2 <= 0 ) {
        nx_fmt = ModeClock::NX_FMT_HMS1;
      } else {
        nx_fmt = ModeClock::NX_FMT_HMS2;
      }
    }
    break;

  case CLOCK_MODE_dHM:
    if ( wl_stat == WL_CONNECTED ) {
      if ( tv->tv_sec % 2 == 0 ) {
        nx_fmt = ModeClock::NX_FMT_dHM1;
      } else {
        nx_fmt = ModeClock::NX_FMT_dHM2;
      }
    } else {
      //if ( tv->tv_sec % 4 <= 2 ) {
      if ( tv->tv_sec % 2 <= 0 ) {
        nx_fmt = ModeClock::NX_FMT_dHM1;
      } else {
        nx_fmt = ModeClock::NX_FMT_dHM2;
      }
    }
    break;

  case CLOCK_MODE_ymd:
    // cbBtn()で、date_start_msが、cur_msを抜かすことがあるので、
    // あらためて cur_ms を取得し直す!
    cur_ms = millis();
    if ( cur_ms - this->date_start_ms > ModeClock::CLOCK_MODE_DATE_INTERVAL ) {
      this->clock_mode = this->clock_mode_main;
      log_i("clock_mode = %d (cur_ms=%u, date_start_ms=%u)",
            this->clock_mode, cur_ms, this->date_start_ms);
      return;
    }

    nx_fmt = ModeClock::NX_FMT_ymd;
    break;

  default:
    break;
  } // switch (clock_mode)
  
  static std::string prev_nx_str = "";

  long nx_usec = tv->tv_usec;
  if ( this->effect[this->conf->effect_i] != NXT_EFFECT_NONE ) {
    // T.B.D. effectする場合、少し進める
    nx_usec + 500000;
  }
  log_v("nx_usec = %d", nx_usec);
  time_t nx_sec = tv->tv_sec + (nx_usec / 1000000);
  struct tm nx_tm;
  localtime_r(&nx_sec, &nx_tm);

  std::string nx_str = tm2string(&nx_tm, nx_fmt);
  log_v("nx_fmt = %s, nx_str = %s", nx_fmt, nx_str.c_str());

  static clock_mode_t prev_clock_mode = CLOCK_MODE_ymd;
  bool force_all = false;
  if ( this->clock_mode != prev_clock_mode ) {
    force_all = true;
    prev_clock_mode = this->clock_mode;
  }

  if ( this->conf->demo_mode ) {
    force_all = true;
  }

  if ( nx_str != prev_nx_str ) {
    unsigned long ms = 30 * (BRIGHTNESS_RESOLUTION / Nxa->brightness());
    Nxa->set_string(nx_str.c_str(), this->effect[this->conf->effect_i],
                    ms, force_all);
  }
  prev_nx_str = nx_str;

  //---------------
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // ModeClock::loop()

/**
 *
 */
void ModeClock::cbBtn(ButtonInfo_t *bi,
                      std::map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( bi->long_pressed ) {
        if ( bi->repeat_count == 0 ) {
          Mode::set("ModeSetclock");
          return;
        }
      }
    }

    // Btn1:OFF
    if ( bi->click_count == 2 ) {
      this->conf->effect_i = (this->conf->effect_i + 1) % this->effect.size();
      this->conf->save();
      return;
    }

    if ( bi->click_count == 3 ) {
      Mode::set("ModeScoreboard");
      return;
    }
    return;
  } // if (Btn0)

  if ( String(bi->name) == "Btn1" ) {
    if ( bi->value == Button::ON ) {
      if ( ! bi->long_pressed ) {
        //
        // Btn1: single short push
        //
        if ( this->clock_mode != CLOCK_MODE_ymd ) {
          this->clock_mode = CLOCK_MODE_ymd;
          this->date_start_ms = millis();
        } else {
          this->clock_mode = this->clock_mode_main;
          this->date_start_ms = 0;
        }
        log_i("clock_mode = %d", this->clock_mode);
        return;
      } else { // long_pressed
        if ( bi->repeat_count == 1 ) {
          //
          // Btn1: long pressed(1)
          //
          if ( this->clock_mode_main == CLOCK_MODE_HMS ) {
            this->clock_mode_main = CLOCK_MODE_dHM;
          } else {
            this->clock_mode_main = CLOCK_MODE_HMS;
          }
          // save main mode
          this->conf->clock_mode = this->clock_mode_main;
          this->conf->save();
            
          this->clock_mode = this->clock_mode_main;
          this->date_start_ms = 0;
          return;
        } // if (repeat_count==0)
      } // if (!long_pressed)
    } // if (ON)

    return;
  } // if (Btn1)

  if ( String(bi->name) == "Btn2" ) {
    if ( bi->value == Button::OFF ) {
      if ( bi->click_count == 1 ) {
        if ( ! bi->long_pressed ) {
          brightness_t bri = Nxa->brightness() / 2;
          if ( bri < BRIGHTNESS_MIN ) {
            bri = BRIGHTNESS_RESOLUTION;
          }
          Nxa->end_all_effect();
          Nxa->set_brightness(bri);
          Nxa->display(millis());
          return;
        } // if ( !long_pressed )
      } // if (click_count == 1 )
    } // if (OFF)

    if ( bi->value == Button::ON ) {
      if ( bi->long_pressed ) {
        if ( bi->repeat_count == 0 ) {
          this->conf->demo_mode = ! this->conf->demo_mode;
          log_i("demo_mode = %s", this->conf->demo_mode ? "true" : "false");

          this->conf->save();
          return;
        } // if (repeat_count == 0)
      } // if (long_pressed)
    } // if (ON)

    return;
  } // if (Btn2)

} // ModeClock::cbBtn()
