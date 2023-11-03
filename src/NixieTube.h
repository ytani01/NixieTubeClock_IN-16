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

#include <map>
#include "NixieElement.h"
#include "NixieEffect.h"

typedef enum {
  NXT_EFFECT_NONE,
  NXT_EFFECT_XFADE,
  NXT_EFFECT_SHUFFLE,
  NXT_EFFECT_FOG,
  NXT_EFFECT_SIZE
} nxt_effect_t;

class NixieTube {
 public:
  int element_n = 0;
  NixieElement *element; // array

  NixieTube();

  void setup(int element_n, uint8_t *pin);
  void loop(unsigned long cur_ms=0);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);

  void end_effect();
  bool effect_is_active();

  std::map<std::string, NixieEffect *> ef;

  void ef_only(int el_i);
  void ef_xfade(int el_src, int el_dst,
                    unsigned long ms, unsigned long start_ms=0);

 private:
  brightness_t _brightness;
}; // class NixieTube
#endif // NIXIE_TUBE_H
