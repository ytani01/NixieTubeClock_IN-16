/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ Data structure ( not a class tree ) ]
 *
 *  Nixie_TubeArray
 *   |
 *   +- Nixie_Tube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- Nixie_Element element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- Nixie_Effect
 *   |
 *   +- Nixie_Tube colon[NIXIE_COLON_N]
 *       |
 *       +- Nixie_Element element[NIXIE_COLON_DOT_N]
 *       |
 *       +- Nixie_Effect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_TUBE_H
#define NIXIE_TUBE_H

#include <map>
#include "Nixie_Element.h"
#include "Nixie_Effect.h"

typedef enum {
  NXT_EFFECT_NONE,
  NXT_EFFECT_XFADE,
  NXT_EFFECT_SHUFFLE,
  NXT_EFFECT_FOG,
  NXT_EFFECT_SIZE
} nxt_effect_t;

class Nixie_Tube {
 public:
  int element_n = 0;
  Nixie_Element *element; // array

  Nixie_Tube();

  void setup(int element_n, uint8_t *pin);
  void loop(unsigned long cur_ms=0);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);

  void end_effect();
  bool effect_is_active();

  std::map<std::string, Nixie_Effect *> ef;

  void ef_only(int el_i);
  void ef_xfade(int el_src, int el_dst,
                    unsigned long ms, unsigned long start_ms=0);

 private:
  brightness_t _brightness;
}; // class Nixie_Tube
#endif // NIXIE_TUBE_H
