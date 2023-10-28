/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeSetclock.h"

/**
 *
 */
ModeSetclock::ModeSetclock(): Mode() {
} // ModeSetclock::ModeSetclock()

/** virtual
 *
 */
void ModeSetclock::enter() {
  log_i("enter mode: %s", this->name.c_str());

  struct tm *tm = SysClock::now_tm();

  this->pos = SETCLOCK_POS_YEAR;
  this->prev_pos = SETCLOCK_POS_N;
  this->flag_nx_update = true;

  this->val.year = (tm->tm_year + 1900) % 100;
  this->val.month = tm->tm_mon + 1;
  this->val.day = tm->tm_mday;
  this->val.hour = tm->tm_hour;
  this->val.minute = tm->tm_min;
  this->val.sec = tm->tm_sec;

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(1);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // ModeSetclock::enter()

/** virtual
 *
 */
void ModeSetclock::exit() {
  Nxa->end_all_effect();
} // ModeSetclock::exit()

/** virtual
 *
 */
void ModeSetclock::loop() {
  unsigned long cur_ms = millis();
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

  struct tm *tm = SysClock::now_tm();
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
  
  Disp->printf("  %s\n", tm2string(tm, fmt_time).c_str());

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

  if ( ! this->flag_nx_update ) {
    delayOrChangeMode(this->LOOP_DELAY_MS);
    return;
  }
  this->flag_nx_update = false;
    
  //
  // NixieTubeArray
  //
  char *nx_fmt = NX_FMT_HMS;
  int num1, num2, num3;

  switch ( this->pos ) {
  case SETCLOCK_POS_YEAR:
  case SETCLOCK_POS_MONTH:
  case SETCLOCK_POS_DAY:
    nx_fmt = ModeSetclock::NX_FMT_ymd;
    num1 = this->val.year;
    num2 = this->val.month;
    num3 = this->val.day;
    break;
  case SETCLOCK_POS_HOUR:
  case SETCLOCK_POS_MINUTE:
  case SETCLOCK_POS_SEC:
    nx_fmt = ModeSetclock::NX_FMT_ymd;
    num1 = this->val.hour;
    num2 = this->val.minute;
    num3 = this->val.sec;
    break;
  default:
    log_e("pos = %d", (int)this->pos);
    break;
  } // switch (clock_mode)
  
  char nx_str[16];
  sprintf(nx_str, nx_fmt, num1, num2, num3);
  log_i("nx_str = %s", nx_str);

  Nxa->end_all_effect();
  Nxa->set_string(nx_str);

  //
  // blink current position
  //
  switch ( this->pos ) {
  case SETCLOCK_POS_YEAR:
  case SETCLOCK_POS_HOUR:
    Nxa->num[0].blink_start(millis(), 200);
    Nxa->num[1].blink_start(millis(), 200);
    break;
  case SETCLOCK_POS_MONTH:
  case SETCLOCK_POS_MINUTE:
    Nxa->num[2].blink_start(millis(), 200);
    Nxa->num[3].blink_start(millis(), 200);
    break;
  case SETCLOCK_POS_DAY:
  case SETCLOCK_POS_SEC:
    Nxa->num[4].blink_start(millis(), 200);
    Nxa->num[5].blink_start(millis(), 200);
    break;
  } // switch(pos)

  //
  this->prev_pos = this->pos;
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // ModeSetclock::loop()

/**
 *
 */
void ModeSetclock::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());

  if ( String(bi->name) == "Btn0" ) {
    if ( bi->value == Button::ON ) {
      if ( bi->long_pressed ) {
        if ( bi->repeat_count == 0 ) {
          Mode::set("ModeClock");
        }
      }
    }
    return;
  } // if (Btn0)

  if ( String(bi->name) == "Btn1" &&
       bi->value == Button::ON ) {

    // XXX enumの計算
    int pos_int = static_cast<int>(this->pos);
    int pos_n = static_cast<int>(SETCLOCK_POS_N);

    pos_int = (pos_int + 1) % pos_n;
    log_i("pos_int = %d", pos_int);

    this->pos = static_cast<setclock_pos_t>(pos_int);

    this->flag_nx_update = true;
    return;
  } // if (Btn1)

  if ( String(bi->name) == "Btn2" &&
       bi->value == Button::ON ) {

    switch ( this->pos ) {
    case SETCLOCK_POS_YEAR:
      this->val.year++;
      if ( this->val.year > this->MAX_YEAR ) {
        this->val.year = this->MIN_YEAR;
      }
      log_i("year = %d", this->val.year);
      break;
    case SETCLOCK_POS_MONTH:
      this->val.month++;
      if ( this->val.month > this->MAX_MONTH ) {
        this->val.month = this->MIN_MONTH;
      }
      log_i("month = %d", this->val.month);
      break;
    } // switch (pos)

    this->flag_nx_update = true;
    return;
  } // if (Btn2)

} // ModeSetclock::cbBtn()
