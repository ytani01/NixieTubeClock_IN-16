/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MY_RTC_
#define _MY_RTC_

#include <RTClib.h>
#include "commonlib.h"

#define MY_RTC RTC_DS3231

/**
 * set Rename super class name
 */
class MyRtc: public MY_RTC {
public:

  void adjust(const DateTime &dt);
  void adjust(struct tm *tm);
}; // class MyRtc

#endif // _MY_RTC_
