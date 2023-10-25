/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "commonlib.h"

static String WDayStr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/**
 * get MAC address String
 */
std::string get_mac_addr_string() {
  uint8_t mac_addr[6];
  char mac_str[13];

  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  sprintf(mac_str, "%02x%02x%02x%02x%02x%02x",
          mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
  //log_v("MacAddr=%s", mac_str);

  return std::string(mac_str);
} // get_mac_addr_string()

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
std::string datetime2string(DateTime *dt, const char *fmt) {
#if 0
  time_t t = (time_t)dt->unixtime();
  log_d("t = %d", t);
  log_d("t2= %d", time(NULL));
  localtime_r(&t, &tm);
  //gmtime_r(&t, &tm);
#endif

  static struct tm tm;
  tm.tm_year = dt->year() - 1900;
  tm.tm_mon = dt->month() - 1;
  tm.tm_mday = dt->day();
  tm.tm_wday = dt->dayOfTheWeek();
  tm.tm_hour = dt->hour();
  tm.tm_min = dt->minute();
  tm.tm_sec = dt->second();
  
  return tm2string(&tm, fmt);
} // datetime2str()
