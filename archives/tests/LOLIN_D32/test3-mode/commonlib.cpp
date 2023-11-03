/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "commonlib.h"

static String WDayStr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/**
 * get MAC address String
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
 * struct tm --> string
 */
char *tm2str(struct tm *tm, const char fmt[]) {
  static char buf[DATETIME_STR_LEN];

  int res = strftime(buf, sizeof(buf), fmt, tm);

  return (char *)buf;
} // tm2str()

/**
 * DateTime --> string
 */
char *datetime2str(DateTime *dt) {
  time_t t = (time_t)dt->unixtime();

  struct tm *tm = localtime(&t);

  return tm2str(tm);
} // datetime2str()
