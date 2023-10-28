/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MY_RTC_
#define _MY_RTC_

#include <RTClib.h>

/**
 * set Rename super class name
 */
class MyRtc: public RTC_DS3231 {
}; // class MyRtc

#endif // _MY_RTC_
