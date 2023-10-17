/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MY_RTC_
#define _MY_RTC_

#include <RTClib.h>
#include "commonlib.h"

/**
 * set Rename super class name
 */
class MyRtc: public RTC_DS3231 {
public:

  void adjust_tm(struct tm *tm);
}; // class MyRtc

#endif // _MY_RTC_
