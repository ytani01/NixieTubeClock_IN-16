/**
 * (c) 2021 Yoichi Tanibayashi
 *
 *------------------------------------------------------------------------
 * [ Data structure ( not a class tree ) ]
 *
 *  NixieArray
 *   |
 *   +- NixieTube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- NixieElement element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- Effect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- Effect
 *-----------------------------------------------------------------------
 */
#ifndef NIXIE_ARRAY_H
#define NIXIE_ARRAY_H
#include "NixieTube.h"

class NixieArray {
 public:
  static const unsigned int DISP_DELAY_US = 1; // microsec
          
  unsigned int brightness = 3;

  NixieTube  num[NIXIE_NUM_N];
  NixieTube  colon[NIXIE_COLON_N];

  NixieArray(uint8_t clk, uint8_t stobe, uint8_t data, uint8_t blank,
             uint8_t num[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N],
             uint8_t colon[NIXIE_COLON_N][NIXIE_COLON_DOT_N]);
  void loop(unsigned long cur_ms);

  void end_all_effect();
  
  void set_onoff(unsigned long cur_ms); // 表示状態更新
  void display(unsigned long cur_ms);

 private:
  uint8_t    _pin_clk, _pin_stobe, _pin_data, _pin_blank;
};

#endif // NIXIE_ARRAY_H
