/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ Data structure ( not a class tree ) ]
 *
 *  NixieTubeArray
 *   |
 *   +- NixieTube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- NixieElement element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- NixieEffect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- NixieEffect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_TUBE_H
#define NIXIE_TUBE_H

#include "NixieElement.h"
#include "NixieEffect.h"

class NixieTube {
 public:
  int           element_n = 0;
  NixieElement *element;

  NixieTube() {};

  void setup(int element_n, uint8_t *pin);
  void loop(unsigned long cur_msec);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);

  NixieEffect *init_effect(effect_id_t eid);
  void end_effect();

  boolean effect_is_active();

  void fadein_start(unsigned long start_ms, unsigned long ms, int el_i);
  void fadeout_start(unsigned long start_ms, unsigned long ms, int el_i);
  void xfade_start(unsigned long start_ms, unsigned long ms,
                   int el_in, int el_out);
  void shuffle_start(unsigned long start_ms, unsigned long tick, int n,
                     int el_i);
  void blink_start(unsigned long start_ms, unsigned long ms);
  void randomOnOff_start(unsigned long start_ms, unsigned long ms, int el_i);

 private:
  NixieEffect *_ef;
  brightness_t _brightness;
}; // class NixieTube
#endif // NIXIE_TUBE_H
