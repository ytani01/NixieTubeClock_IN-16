/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "MainMode.h"

/** constructor
 *
 */
MainMode::MainMode(String name, CommonData_t *common_data)
  : ModeBase(name, common_data) {

  strcpy(this->mac_addr_str, get_mac_addr_String().c_str());
  log_i("mac_addr_str=\"%s\"", this->mac_addr_str);
} // MainMode::MainMode()

/**
 *
 */
Mode_t MainMode::reBtn_cb(ButtonInfo_t *bi) {
  if ( bi->click_count > 0 ) {
    return MODE_MENU;
  }
  return MODE_N;
} // MainMode::reBtn_cb()

/**
 *
 */
Mode_t MainMode::obBtn_cb(ButtonInfo_t *bi) {
  if ( bi->click_count > 0 ) {
    _cd->msg = " Onboard Btn\n";
    _cd->msg += " click:" + String(bi->click_count);
  }
  return MODE_N;
} // MainMode::obBtn_cb()

/**
 *
 */
void MainMode::display(Display_t *disp) {
  int x, y;

  //disp->drawRect(0,0, DISPLAY_W, DISPLAY_H, WHITE);
  disp->setTextWrap(false);
  disp->setTextSize(1);
  disp->setTextColor(WHITE, BLACK);
  
  // frame
  //disp->drawFastHLine(0, 24, DISPLAY_W, WHITE);
  //disp->drawFastHLine(0, 41, DISPLAY_W, WHITE);

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
} // MainMode::display()

/**
 *
 */
void MainMode::drawWiFi(Display_t *disp, int x, int y, NetMgrInfo_t *ni) {
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

  case NETMGR_MODE_TRY_WIFI:
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
} // MainMode::drawWiFi()

/**
 *
 */
void MainMode::drawDateTime(Display_t *disp, int x, int y, struct tm *ti) {
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
} // MainMode::drawDateTime()

/**
 *
 */
void MainMode::drawNtp(Display_t *disp, int x, int y,
                       NtpTaskInfo_t *ntp_info,
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

} // MainMode::drawNtp()  
