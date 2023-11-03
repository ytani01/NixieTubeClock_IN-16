/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef MODE_COUNT_H
#define MODE_COUNT_H
#include "ModeBase.h"
#include "CountData.h"

class ModeCount : public ModeBase {
 public:
  static const unsigned long MODIFY_TIMEOUT = 5000; // ms

  static const unsigned long TICK_MS        = 100; // ms
  static const unsigned long BLINK_TICK_MS  = 300; // ms

  static const mode_t MODE_NONE   = 0x00;
  static const mode_t MODE_MODIFY = 0x01;

  static const int COUNT_N = 3;

  CountData data;
  
  ModeCount(NixieArray *nxa);
  void init(unsigned long start_ms, DateTime& now, int init_val[NIXIE_NUM_N]);
  stat_t loop(unsigned long cur_ms, DateTime& now);
  void btn_intr_hdr(unsigned long cur_ms, Button *btn);
  void btn_loop_hdr(unsigned long cur_ms, Button *btn);

  void get_disp_str(char* disp_str);

 private:
  mode_t _mode;
  int _cur;
  unsigned long _modify_ms;
};
#endif // MODE_COUNT_H
