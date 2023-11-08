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
#ifndef NIXIE_ELEMENT_H
#define NIXIE_ELEMENT_H

#include "Arduino.h"
#include "Nixie.h"

class Nixie_Element {
 public:
  Nixie_Element() {};
  
  void setup(uint8_t pin);
  
  brightness_t max_brightness();
  brightness_t set_max_brightness(brightness_t bri);
  brightness_t set_brightness_to_max();

  brightness_t set_brightness(brightness_t brightness);
  brightness_t inc_brightness();
  brightness_t dec_brightness();
  brightness_t brightness();

  void set_pin(uint8_t pin);
  uint8_t get_pin();
  
  void on();
  void off();
  boolean is_on();
  
 protected:
  uint8_t _pin = 0;
  brightness_t _brightness = 8;
  brightness_t _max_brightness = 8;
  boolean _on = false;

}; // class Nixie_Element
#endif // NIXIE_ELEMENT_H
