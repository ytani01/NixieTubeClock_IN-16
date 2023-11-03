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
 * @brief struct tm --> DateTime
 */
DateTime tm2datetime(struct tm *tm) {
  return DateTime(tm->tm_year + 1900,
                  tm->tm_mon + 1,
                  tm->tm_mday,
                  tm->tm_hour, tm->tm_min, tm->tm_sec);
} // tm2datetime()

/**
 * @brief  struct tm --> string
 *
 * buf が static なので危険！
 * tm2string() を推奨
 */
static char *tm2str(struct tm *tm, const char fmt[]) {
  static char buf[DATETIME_STR_LEN];

  int res = strftime(buf, sizeof(buf), fmt, tm);

  return (char *)buf;
} // tm2str()

/**
 *
 */
std::string tm2string(struct tm *tm, const char fmt[]) {
  return std::string(tm2str(tm, fmt));
}

/**
 * @brief  DateTime --> string
 */
std::string datetime2string(DateTime *dt) {
  time_t t = (time_t)dt->unixtime();
  struct tm *tm = localtime(&t);
  return tm2string(tm);
} // datetime2str()
