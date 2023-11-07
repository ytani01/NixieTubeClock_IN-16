/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_SETCLOCK_H_
#define _MODE_SETCLOCK_H_

#include "Mode.h"

typedef enum {
  SETCLOCK_POS_YEAR,
  SETCLOCK_POS_MONTH,
  SETCLOCK_POS_DAY,
  SETCLOCK_POS_HOUR,
  SETCLOCK_POS_MINUTE,
  SETCLOCK_POS_SEC,
  SETCLOCK_POS_N
} setclock_pos_t;

typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int sec;
} setclock_val_t;

class ModeSetclock: public Mode {
 public:
  static const unsigned long LOOP_DELAY_MS = 50; // ms
  static const unsigned long BLINK_INTERVAL_MS = 300; // ms

  static constexpr char* NX_FMT_ymd = (char *)"%02d %02d %02d";
  static constexpr char* NX_FMT_HMS = (char *)"%02d:%02d:%02d";

  static const int VAL_IDX_YEAR = 0;
  static const int VAL_IDX_MONTH = 1;
  static const int VAL_IDX_DAY = 2;
  static const int VAL_IDX_HOUR = 3;
  static const int VAL_IDX_MINUTE = 4;
  static const int VAL_IDX_SEC = 5;
  static const int VAL_IDX_MAX = 6;
  
  int val[VAL_IDX_MAX];
  int val_min[VAL_IDX_MAX] = {20,  1,  1,  0,  0,  0};
  int val_max[VAL_IDX_MAX] = {40, 12, 31, 23, 59, 59};

  setclock_pos_t pos = SETCLOCK_POS_YEAR;
  setclock_pos_t prev_pos = SETCLOCK_POS_YEAR;
  bool flag_nx_update = false;
      
  unsigned long start_ms = 0;
  static const unsigned long IDLE_INTERVAL_MS = 60 * 1000; // ms
  
  ModeSetclock();

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info);
};

#endif // _MODE_SETCLOCK_H_
