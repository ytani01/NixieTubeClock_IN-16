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
 *   |   +- Effect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- Effect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_ELEMENT_H
#define NIXIE_ELEMENT_H

#include "Arduino.h"
#include "Nixie.h"

class NixieElement {
 public:
  NixieElement() {};
  
  void setup(uint8_t pin);
  
  void set_brightness(uint8_t brightness);
  void inc_brightness();
  void dec_brightness();
  unsigned int get_brightness();

  
  void set_pin(uint8_t pin);
  uint8_t get_pin();
  
  void on();
  void off();
  boolean is_on();
  
 protected:
  uint8_t _pin = 0;
  unsigned int _brightness = 8;
  boolean _on = false;

}; // class NixieElement
#endif // NIXIE_ELEMENT_H
