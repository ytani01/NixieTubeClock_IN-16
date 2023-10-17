/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "MyRtc.h"

/**
 *
 */
void MyRtc::adjust(const DateTime &dt) {
  MY_RTC::adjust(dt);
}

/**
 *
 */
void MyRtc::adjust(struct tm *tm) {
  DateTime dt = tm2datetime(tm);
  this->adjust(dt);
} // MyRtc::adjust()
