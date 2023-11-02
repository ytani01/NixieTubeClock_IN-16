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
  NixieElement *element; // array

  NixieTube();

  void setup(int element_n, uint8_t *pin);
  void loop(unsigned long cur_ms=0);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);

  void end_effect();
  bool effect_is_active();

  NixieEffectFadeIn *ef_fadein;
  NixieEffectFadeOut *ef_fadeout;
  NixieEffectFog *ef_fog;
  NixieEffectShuffle *ef_shuffle;
  NixieEffectBlink *ef_blink;

  void effect_one(int el_i);
  void effect_fadein(int el_i, unsigned long ms, unsigned long start_ms=0);
  void effect_fadeout(int el_i, unsigned long ms, unsigned long start_ms=0);
  void effect_xfade(int el_src, int el_dst,
                    unsigned long ms, unsigned long start_ms=0);
  void effect_fog(int el_i, unsigned long ms, unsigned long start_ms=0);
  void effect_shuffle(int el_i, int n, unsigned long ms, unsigned long start_ms=0);
  void effect_blink(unsigned long ms, unsigned long start_ms=0);

 private:
  NixieEffect *_ef1 = NULL;
  NixieEffect *_ef2 = NULL;
  brightness_t _brightness;
}; // class NixieTube
#endif // NIXIE_TUBE_H
