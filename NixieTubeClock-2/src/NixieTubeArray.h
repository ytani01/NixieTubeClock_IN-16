/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *------------------------------------------------------------------------
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
 *-----------------------------------------------------------------------
 */
#ifndef _NIXIE_TUBE_ARRAY_H_
#define _NIXIE_TUBE_ARRAY_H_

#include "NixieTube.h"
#include "ConfFile_Brightness.h"

typedef enum {
  NXA_EFFECT_NONE,
  NXA_EFFECT_XFADE,
  NXA_EFFECT_SHUFFLE,
  NXA_EFFECT_FOG,
  NXA_EFFECT_SIZE
} nxa_effect_t;

class NixieTubeArray {
 public:
  static const unsigned int DISP_DELAY_US = 1; // microsec
          
  NixieTube  num[NIXIE_NUM_N];
  NixieTube  colon[NIXIE_COLON_N];

  uint8_t prev_num_int[NIXIE_NUM_N];
  uint8_t prev_col_int[NIXIE_COLON_N];

  NixieTubeArray(uint8_t clk, uint8_t stobe, uint8_t data, uint8_t blank,
                 uint8_t num[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N],
                 uint8_t colon[NIXIE_COLON_N][NIXIE_COLON_DOT_N]);
  void loop(unsigned long cur_ms);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);
                              
  void end_all_effect();

  void set_num(uint8_t (&num)[NIXIE_NUM_N],
               nxa_effect_t effect=NXA_EFFECT_NONE,
               unsigned long ms=0, bool force_all=false);

  void set_col(uint8_t (&col)[NIXIE_COLON_N],
               nxa_effect_t effect=NXA_EFFECT_NONE,
               unsigned long ms=0, bool force_all=false);

  void set_string(std::string str,
                  nxa_effect_t effect=NXA_EFFECT_NONE,
                  unsigned long ms=0, bool force_all=false);
  
  void display(unsigned long cur_ms);

 private:
  uint8_t _pin_clk, _pin_stobe, _pin_data, _pin_blank;
  brightness_t _brightness = 3;
  ConfFile_Brightness *_cf_bri;

  void set_onoff(unsigned long cur_ms); // 表示状態更新
};

#endif // _NIXIE_TUBE_ARRAY_H_
