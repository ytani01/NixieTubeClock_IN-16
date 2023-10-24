/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeA.h"

/**
 *
 */
ModeA::ModeA(): Mode() {
} // ModeA::ModeA()

/** virtual
 *
 */
void ModeA::enter() {
  log_i("enter mode: %s", this->name.c_str());

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // ModeA::enter()

/** virtual
 *
 */
void ModeA::exit() {
  Nxa->end_all_effect();
} // ModeA::exit()

/** virtual
 *
 */
void ModeA::loop() {
  unsigned long cur_ms = millis();
  struct tm *tm = SysClock::now_tm();
  char *fmt_date, *fmt_time;

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
  // date time format string
  //
  fmt_date = (char *)"%Y/%m/%d(%a)";
  struct timeval *tv = SysClock::now_timeval();
  //if ( tv->tv_usec % 1000000 < 500000 ) {
  if ( tv->tv_sec % 2 == 0 ) {
    fmt_time =(char *)"%H:%M:%S";
  } else {
    fmt_time =(char *)"%H %M %S";
  }

  //
  // for NixieTubeArray
  //
  Nxa->set(tm2string(tm, fmt_time));

  //
  // for Disp
  //
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
