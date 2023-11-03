/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "SysClock.h"

/** static
 *
 */
struct tm* SysClock::now_tm() {
  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);
  // log_d("Sys : %s", tm2string(tm_now).c_str());
  return tm_now;
} // SysClock::get_tm()

/** static
 *
 */
std::string SysClock::now_string(const char fmt[]) {
  struct tm *tm = SysClock::now_tm();
  return tm2string(tm, fmt);
} // SysClock::now_str()

/** static
 *
 */
struct timeval* SysClock::now_timeval() {
  static struct timeval tv;
  gettimeofday(&tv, NULL);
  return &tv;
} // SysClock::now_timeval()

/** static
 *
 */
void SysClock::set(struct tm *tm) {
  struct timeval tv = { mktime(tm), 0 };
  settimeofday(&tv, NULL);
} // SysClock::set()

/** static
 *
 */
void SysClock::set(int year, int month, int day,
                   int hour, int minute, int second) {
  struct tm tm;

  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;

  SysClock::set(&tm);
} // SysClock::set()

/** static
 *
 */
void SysClock::set(DateTime *dt) {
  SysClock::set(dt->year(), dt->month(), dt->day(),
                dt->hour(), dt->minute(), dt->second());
} // SysClock::set()
