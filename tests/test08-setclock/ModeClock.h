/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_CLOCK_H_
#define _MODE_CLOCK_H_

#include "Mode.h"

typedef enum {
  CLOCK_MODE_HMS,
  CLOCK_MODE_dHM,
  CLOCK_MODE_ymd,
  CLOCK_MODE_SIZE
} clock_mode_t;

class ModeClock: public Mode {
 public:
  static constexpr char* NX_FMT_HMS1 = (char *)"%H:%M:%S";
  static constexpr char* NX_FMT_HMS2 = (char *)"%H %M %S";

  static constexpr char* NX_FMT_dHM1 = (char *)"%d %H:%M";
  static constexpr char* NX_FMT_dHM2 = (char *)"%d %H %M";

  static constexpr char* NX_FMT_ymd = (char *)"%y %m %d";

  clock_mode_t clock_mode = CLOCK_MODE_HMS;
  clock_mode_t clock_mode_main = CLOCK_MODE_HMS;

  unsigned long date_start_ms = 0;
  static const unsigned long CLOCK_MODE_DATE_INTERVAL = 5000;
  
  ModeClock();

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi);
};

#endif // _MODE_CLOCK_H_
