/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Main.h"

/** constructor
 *
 */
Mode_Main::Mode_Main(String name, CommonData_t *common_data)
  : Mode(name, common_data) {

  strcpy(this->mac_addr_str, get_mac_addr_String().c_str());
  log_i("mac_addr_str=\"%s\"", this->mac_addr_str);
} // Mode_Main::Mode_Main()

/**
 *
 */
Mode_t Mode_Main::btnCb_Mode(ButtonInfo_t *bi) {
  log_i("%s", this->name.c_str());

  if ( bi->click_count > 0 ) {
    _cd->msg = String(bi->name) + "> click:" + String(bi->click_count);
  }
  return MODE_N;
} // Mode_Main::btnCb_Mode()

/**
 *
 */
Mode_t Mode_Main::btnCb_Up(ButtonInfo_t *bi) {
  log_i("%s", this->name.c_str());
  
  if ( bi->click_count > 0 ) {
    _cd->msg = String(bi->name) + "> click:" + String(bi->click_count);
  }
  return MODE_N;
} // Mode_Main::btnCb_Up()

/**
 *
 */
Mode_t Mode_Main::btnCb_Down(ButtonInfo_t *bi) {
  (void)Mode::btnCb_Down(bi);

  if ( bi->click_count > 0 ) {
    _cd->msg = String(bi->name) + "> click:" + String(bi->click_count);
  }
  return MODE_N;
} // Mode_Main::btnCb_Down()

/**
 *
 */
void Mode_Main::setup() {
  log_i("%s", this->name.c_str());

} // Mode_Main::setup()

/**
 *
 */
bool Mode_Main::enter(Mode_t prev_mode) {
  log_i("%s: prev_mode=%s", this->name.c_str(), MODE_T_STR[prev_mode]);
  this->prev_mode = prev_mode;

  Nta->brightness = BRIGHTNESS_RESOLUTION / 4;

  int init_val[NIXIE_NUM_N] = {0, 1, 2, 3, 4, 5};
  for (int i = 0; i < NIXIE_NUM_N; i++) {
    NtaNum(i).end_effect();

    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( init_val[i] == e ) {
        //Nta->num[i].element[e].set_brightness(Nta->brightness);
        NtaNumEl(i,e).set_brightness(Nta->brightness);
      } else {
        //Nta->num[i].element[e].set_brightness(0);
        NtaNumEl(i,e).set_brightness(0);
      }
    } // for(e)

    NtaNum(i).blink_start(millis(), 1000);
  } // for(i)

  for (int i=0; i < NIXIE_COLON_N; i++) {
    Nta->colon[i].element[0].set_brightness(Nta->brightness);
  } // for(i)
  
  return true;
}

/**
 * モード切替時に毎回実行
 */
bool Mode_Main::exit() {
  log_i("%s", this->name.c_str());
  return true;
} // Mode::resume()

/**
 *
 */
void Mode_Main::loop(unsigned long cur_ms) {
  static int prev_num = 0;
  int num = cur_ms / 3000 % 10;
  int nixienum = 5;
  static unsigned long FADE_TICK = 30;
  unsigned long fade_tick
    = FADE_TICK * ( BRIGHTNESS_RESOLUTION / Nta->brightness );
  
  if ( num != prev_num ) {
    NtaNum(nixienum).end_effect();

    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( e == num ) {
        NtaNumEl(nixienum, e).set_brightness(common_data->nta->brightness);
      } else {
        NtaNumEl(nixienum, e).set_brightness(0);
      }
    }

    NtaNum(nixienum).xfade_start(millis(), fade_tick, num, prev_num);

    prev_num = num;
  }
} // Mode_Main::loop()

/**
 *
 */
void Mode_Main::display(Display_t *disp) {
  int x, y;

  //disp->drawRect(0,0, DISPLAY_W, DISPLAY_H, WHITE);
  disp->setTextWrap(false);
  disp->setTextSize(1);
  disp->setTextColor(WHITE, BLACK);
  
  x = 0;
  y = 0;

  // Date/Time
  time_t t_now = time(NULL);
  struct tm *ti = localtime(&t_now);
  x = 0;
  y = 0;
  this->drawDateTime(disp, x, y, ti);

  // NTP
  x = DISPLAY_W - 4 * 3;
  y = DISPLAY_CH_H / 2;
  this->drawNtp(disp, x, y, _cd->ntp_info, _cd->netmgr_info);

  // WiFi
  x = 0;
  y = DISPLAY_H - DISPLAY_CH_H * 2 - 1;
  this->drawWiFi(disp, x, y, _cd->netmgr_info);

  // MAC Addr
  x = DISPLAY_W - DISPLAY_CH_W * 12;
  y = DISPLAY_H - DISPLAY_CH_H;
  disp->setFont(NULL);
  disp->setTextSize(1);
  disp->setCursor(x, y);
  disp->printf("%s ", this->mac_addr_str);
} // Mode_Main::display()

/**
 *
 */
void Mode_Main::drawWiFi(Display_t *disp, int x, int y, NetMgrInfo_t *ni) {
  disp->setFont(NULL);
  disp->setTextSize(1);
  disp->setCursor(x, y);

  int interval, ms;

  switch ( ni->mode ) {
  case NETMGR_MODE_START:
    if ( millis() % 500 < 500 * 70 / 100 ) {
      disp->printf("Starting WiFi");
    }
    break;

  case NETMGR_MODE_WAIT_CONNECT:
    if ( millis() % 500 < 500 * 70 / 100 ) {
      disp->printf("%s", ni->ssid.c_str());
    }
    break;

  case NETMGR_MODE_WIFI_ON:
    disp->printf("%s", ni->ssid.c_str());

    disp->setFont(&Picopixel);
    disp->setCursor(0, DISPLAY_H - 2);
    disp->printf("%s", ni->ip_addr.toString().c_str());
    break;

  case NETMGR_MODE_AP_INIT:
  case NETMGR_MODE_AP_LOOP:
    if ( millis() % 3000 < 3000 * 95 / 100 ) {
      disp->printf("[%s]", ni->ap_ssid.c_str());
    }
    break;

  case NETMGR_MODE_WIFI_OFF:
    if ( millis() % 500 < 500 * 50 / 100 ) {
      disp->printf("%s", ni->ssid.c_str());
    }
    break;
  } // switch(ni->mode)
} // Mode_Main::drawWiFi()

/**
 *
 */
void Mode_Main::drawDateTime(Display_t *disp, int x, int y, struct tm *ti) {
  if ( ti->tm_year + 1900 < 2000 ) {
    return;
  }

  char wday_str[4];
  strftime(wday_str, sizeof(wday_str), "%a", ti);

  disp->setFont(NULL);
  disp->setTextSize(1);
  disp->setCursor(x, y);

  if ( millis() % 1000 < 500 ) {
    disp->printf("%02d/%02d %s %02d:%02d:%02d",
                 ti->tm_mon + 1, ti->tm_mday, wday_str,
                 ti->tm_hour, ti->tm_min, ti->tm_sec);
  } else {
    disp->printf("%02d/%02d %s %02d %02d %02d",
                 ti->tm_mon + 1, ti->tm_mday, wday_str,
                 ti->tm_hour, ti->tm_min, ti->tm_sec);
  }
} // Mode_Main::drawDateTime()

/**
 *
 */
void Mode_Main::drawNtp(Display_t *disp, int x, int y,
                       Task_NtpInfo_t *ntp_info,
                       NetMgrInfo_t *netmgr_info) {
  unsigned long interval_ms = 500;
  unsigned long on_rate = 50;

  switch ( ntp_info->sntp_stat ) {
  case SNTP_SYNC_STATUS_RESET:
    interval_ms = 500;
    on_rate = 50;
    break;
  case SNTP_SYNC_STATUS_IN_PROGRESS:
    interval_ms = 2000;
    on_rate = 80;
    break;
  case SNTP_SYNC_STATUS_COMPLETED:
    interval_ms = 1000;
    on_rate = 100;
    break;
  default:
    break;
  } // switch (sntp_stat)
  if ( netmgr_info->mode != NETMGR_MODE_WIFI_ON ) {
    interval_ms = 2000;
    on_rate = 10;
  }

  //  x = DISPLAY_W - DISPLAY_CH_W * 3;
  //  y = 27;
  disp->setFont(&Picopixel);
  disp->setTextSize(1);
  disp->setCursor(x, y);
  if ( millis() % interval_ms < interval_ms * on_rate / 100 ) {
    disp->printf("NTP");
  }

} // Mode_Main::drawNtp()  
