/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef MODE_CLOCK_H
#define MODE_CLOCK_H
#include "ModeBase.h"

/**
 *
 */
class ModeClock : public ModeBase {
 public:
  static const unsigned long MODE_NULL = 0x00;
  static const unsigned long MODE_HMS  = 0x01;
  static const unsigned long MODE_DHM  = 0x02;
  static const unsigned long MODE_YMD  = 0x03;
  static const unsigned long MODE_MDH  = 0x04;

  static const unsigned long DISP_DATE_MS = 7000;  // ms

  static const unsigned long TICK_MS      = 10;    // ms
  static const unsigned long FADE_TICK_MS = 30;    // ms

  unsigned long mode = MODE_HMS;
  unsigned long mode_start_ms = 0;
  static const unsigned long CH_MODE_MS_LIMIT = 2000;
  unsigned int  brightness = BRIGHTNESS_RESOLUTION;

  ModeClock(NixieArray *nxa);
  void init(unsigned long start_ms, DateTime& now, int init_val[NIXIE_NUM_N]);
  stat_t loop(unsigned long cur_ms, DateTime& now);
  void change_mode(unsigned long mode);
  void btn_intr_hdr(unsigned long cur_ms, Button *btn);
  void btn_loop_hdr(unsigned long cur_ms, Button *btn);
};
#endif // MODE_CLOCK_H
