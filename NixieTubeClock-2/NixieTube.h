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

  bool effect_is_active();

  void one(int el_i);
  void fadein_start(int el_i, unsigned long ms, unsigned long start_ms=0);
  void fadeout_start(int el_i, unsigned long ms, unsigned long start_ms=0);
  void xfade_start(int el_src, int el_dst,
                   unsigned long ms, unsigned long start_ms=0);
  void shuffle_start(int el_i, int n, unsigned long ms, unsigned long start_ms=0);
  void blink_start(unsigned long ms, unsigned long start_ms=0);
  void randomOnOff_start(int el_i, unsigned long ms, unsigned long start_ms=0);

 private:
  NixieEffect *_ef = NULL;
  NixieEffect *_ef2 = NULL;
  brightness_t _brightness;
}; // class NixieTube
#endif // NIXIE_TUBE_H
