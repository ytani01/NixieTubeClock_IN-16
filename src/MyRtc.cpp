/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "MyRtc.h"

/**
 *
 */
DateTime MyRtc::now() {
  bool prev_flag_intr = Flag_enableIntr;
  
  disableIntr();

  DateTime dt = MY_RTC::now();

  if ( prev_flag_intr ) {
    enableIntr();
  }

  return dt;
} // MyRtc::now()

/**
 *
 */
void MyRtc::adjust(const DateTime &dt) {
  bool prev_flag_intr = Flag_enableIntr;

  disableIntr();

  MY_RTC::adjust(dt);

  if ( prev_flag_intr ) {
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
