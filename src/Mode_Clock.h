/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_CLOCK_H_
#define _MODE_CLOCK_H_

#include "Mode.h"
#include "ConfFile_ModeClock.h"

typedef enum {
  CLOCK_MODE_HMS,
  CLOCK_MODE_dHM,
  CLOCK_MODE_ymd,
  CLOCK_MODE_SIZE
} clock_mode_t;

class Mode_Clock: public Mode {
 public:
  static const unsigned long LOOP_DELAY_MS = 50; // ms

  static constexpr char* NX_FMT_HMS1 = (char *)"%H:%M:%S";
  static constexpr char* NX_FMT_HMS2 = (char *)"%H %M %S";

  static constexpr char* NX_FMT_dHM1 = (char *)"%d %H:%M";
  static constexpr char* NX_FMT_dHM2 = (char *)"%d %H %M";

  static constexpr char* NX_FMT_ymd = (char *)"%y %m %d";

  clock_mode_t clock_mode = CLOCK_MODE_HMS;
  clock_mode_t clock_mode_main = CLOCK_MODE_HMS;

  unsigned long date_start_ms = 0;
  static const unsigned long CLOCK_MODE_DATE_INTERVAL = 5000;

  ConfFile_ModeClock *conf;

  bool demo_mode = false;

  std::vector<nxt_effect_t> effect = {
    NXT_EFFECT_XFADE,
    NXT_EFFECT_NONE,
    NXT_EFFECT_FOG,
    NXT_EFFECT_SHUFFLE
  };
  
  Mode_Clock();

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info);
};

#endif // _MODE_CLOCK_H_
