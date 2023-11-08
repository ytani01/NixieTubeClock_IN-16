/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *------------------------------------------------------------------------
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
 *-----------------------------------------------------------------------
 */
#ifndef _NIXIE_TUBE_ARRAY_H_
#define _NIXIE_TUBE_ARRAY_H_

#include "Nixie_Tube.h"
#include "ConfFile_Brightness.h"

class Nixie_TubeArray {
 public:
  static const unsigned int DISP_DELAY_US = 1; // microsec
          
  Nixie_Tube  num[NIXIE_NUM_N];
  Nixie_Tube  colon[NIXIE_COLON_N];

  uint8_t prev_num_int[NIXIE_NUM_N];
  uint8_t prev_col_int[NIXIE_COLON_N];

  Nixie_TubeArray(uint8_t clk, uint8_t stobe, uint8_t data, uint8_t blank,
                 uint8_t num[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N],
                 uint8_t colon[NIXIE_COLON_N][NIXIE_COLON_DOT_N]);
  void loop(unsigned long cur_ms);

  brightness_t brightness();
  brightness_t set_brightness(brightness_t bri);
                              
  void end_all_effect();

  void set_num(uint8_t (&num)[NIXIE_NUM_N],
               nxt_effect_t effect=NXT_EFFECT_NONE,
               unsigned long ms=0, bool force_all=false);

  void set_col(uint8_t (&col)[NIXIE_COLON_N],
               nxt_effect_t effect=NXT_EFFECT_NONE,
               unsigned long ms=0, bool force_all=false);

  void set_string(std::string str,
                  nxt_effect_t effect=NXT_EFFECT_NONE,
                  unsigned long ms=0, bool force_all=false);
  
  void display(unsigned long cur_ms);

 private:
  uint8_t _pin_clk, _pin_stobe, _pin_data, _pin_blank;
  brightness_t _brightness = 3;
  ConfFile_Brightness *_cf_bri;

  void set_onoff(unsigned long cur_ms); // 表示状態更新
};

#endif // _NIXIE_TUBE_ARRAY_H_
