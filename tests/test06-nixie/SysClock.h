/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _SYSCLOCK_
#define _SYSCLOCK_

#include <time.h>
#include "commonlib.h"

class SysClock {
 public:
  static struct tm* now_tm();
  static std::string now_string(const char fmt[]="%Y/%m/%d(%a) %H:%M:%S");
  static struct timeval* now_timeval();

  static void set(struct tm *tm);
  static void set(int year, int month, int day,
                  int hour, int minute, int second);
  static void set(DateTime *dt);
}; // class SysClock

#endif // _SYSCLOCK_
