/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef MODE_TEST1_H
#define MODE_TEST1_H
#include "ModeBase.h"

class ModeTest1 : public ModeBase {
 public:
  static const unsigned long TICK_MS = 1000; // ms
  static const unsigned long FADE_TICK_MS = 50; // ms
  static const unsigned long BLINK_TICK_MS = 100; // ms
  static const unsigned long SHUFFLE_TICK_MS = 30; // ms
  static const unsigned long SHUFFLE_COUNT = 20;
  
  ModeTest1(NixieArray *nxa);
  void init(unsigned long start_ms, DateTime& now,
            int init_val[NIXIE_NUM_N]);
  stat_t loop(unsigned long cur_ms, DateTime& now);
  void btn_intr_hdr(unsigned long cur_ms, Button *btn);
  void btn_loop_hdr(unsigned long cur_ms, Button *btn);

 private:
  int _digit;
  int _prev_digit;
};
#endif // MODE_TEST1_H
