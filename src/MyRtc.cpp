/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "MyRtc.h"

/**
 *
 */
DateTime MyRtc::now() {
  disableIntr();

  DateTime dt = MY_RTC::now();

  if ( Flag_enableIntr ) {
    enableIntr();
  }

  return dt;
} // MyRtc::now()

/**
 *
 */
void MyRtc::adjust(const DateTime &dt) {
  disableIntr();

  MY_RTC::adjust(dt);

  if ( Flag_enableIntr ) {
    enableIntr();
  }
} // MyRtc::adjust()

/**
 *
 */
void MyRtc::adjust(struct tm *tm) {
  DateTime dt = tm2datetime(tm);
  this->adjust(dt);
} // MyRtc::adjust()
