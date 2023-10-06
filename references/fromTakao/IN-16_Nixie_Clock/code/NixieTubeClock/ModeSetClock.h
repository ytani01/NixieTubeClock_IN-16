/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef MODE_SET_CLOCK_H
#define MODE_SET_CLOCK_H
#include "ModeBase.h"
#include "PCF8563.h"

class ModeSetClock : public ModeBase {
 public:
  static const mode_t MODE_NULL   = 0x00;

  static const mode_t MODE_DATE   = 0x10;
  static const mode_t MODE_YEAR   = 0x11;
  static const mode_t MODE_MONTH  = 0x12;
  static const mode_t MODE_DAY    = 0x13;

  static const mode_t MODE_TIME   = 0x20;
  static const mode_t MODE_HOUR   = 0x21;
  static const mode_t MODE_MINUTE = 0x22;

  static const mode_t TICK_MS = 10;        // ms
  static const mode_t BLINK_TICK_MS = 200; // ms

  ModeSetClock(NixieArray *nxa);
  void init(unsigned long start_ms, DateTime& now, int init_val[NIXIE_NUM_N]);
  stat_t loop(unsigned long cur_ms, DateTime& now);
  void btn_intr_hdr(unsigned long cur_ms, Button *btn);
  void btn_loop_hdr(unsigned long cur_ms, Button *btn);
  void attachI2C(TwoWire *wire);

  mode_t change_mode(mode_t mode);
  void count_up(int n, boolean repeat);
  TwoWire *_wire;

 private:
  mode_t _mode;
  int    _date_num[NIXIE_NUM_N + 1];
  int    _time_num[NIXIE_NUM_N + 1];
  
}; // class ModeSetClock
#endif // MODE_SET_CLOCK_H
