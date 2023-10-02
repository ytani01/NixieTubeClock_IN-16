/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef MODE_TEST2_H
#define MODE_TEST2_H
#include "ModeBase.h"

class ModeTest2 : public ModeBase {
 public:
  static const unsigned long TICK_MS = 4000; // ms
  static const unsigned long FADE_TICK_MS = 90; // ms
  static const unsigned long SHUFFLE_TICK_MS = 10; // ms
  static const unsigned long SHUFFLE_COUNT = 60;
  
  ModeTest2(NixieArray *nxa);
  void init(unsigned long start_ms, DateTime& now,
            int init_val[NIXIE_NUM_N]);
  stat_t loop(unsigned long cur_ms, DateTime& now);
  void btn_intr_hdr(unsigned long cur_ms, Button *btn);
  void btn_loop_hdr(unsigned long cur_ms, Button *btn);

 private:
  int _digit[NIXIE_NUM_N];
  int _cur;
};
#endif // MODE_TEST2_H
