/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "common.h"

static String WDayStr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/**
 *
 */
String get_mac_addr_String() {
  uint8_t mac_addr[6];
  char mac_str[13];

  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  sprintf(mac_str, "%02x%02x%02x%02x%02x%02x",
          mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
  log_d("MacAddr=%s", mac_str);

  return String(mac_str);
} // get_mac_addr_String()

/**
 *
 */
char *tm2str(struct tm *tm) {
  static char str[64];

  int res = strftime(str, sizeof(str), "%Y/%m/%d(%a) %H:%M:%S", tm);

  return (char *)str;
} // tm2str()

/**
 *
 */
char *datetime2str(DateTime *dt) {
  static struct tm tm;

  tm.tm_year = dt->year() - 1900;
  tm.tm_mon = dt->month() - 1;
  tm.tm_mday = dt->day();
  tm.tm_hour = dt->hour();
  tm.tm_min = dt->minute();
  tm.tm_sec = dt->second();

  return tm2str(&tm);
} // datetime2str()
