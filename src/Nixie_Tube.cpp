/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Nixie_Tube.h"

/**
 *
 */
Nixie_Tube::Nixie_Tube() {
} // constructor

/**
 *
 */
void Nixie_Tube::setup(int element_n, uint8_t *pin) {
  this->element_n = element_n;
  this->element = new Nixie_Element[this->element_n];
  
  for (int d=0; d < element_n; d++) {
    this->element[d].setup(pin[d]);
  } // for (d)

  this->ef["fadein"] = new Nixie_EffectFadeIn(this->element, this->element_n);
  this->ef["fadeout"] = new Nixie_EffectFadeOut(this->element, this->element_n);
  this->ef["fog"] = new Nixie_EffectFog(this->element, this->element_n);
  this->ef["shuffle"] = new Nixie_EffectShuffle(this->element, this->element_n);
  this->ef["blink"] = new Nixie_EffectBlink(this->element, this->element_n);
} // Nixie_Tube::setup()

/**
 *
 */
void Nixie_Tube::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }

  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      e.second->loop(cur_ms);
    }
  }  
} // Nixie_Tube::loop()

/**
 *
 */
brightness_t Nixie_Tube::brightness() {
  return this->_brightness;
} // Nixie_Tube::brightness()

/**
 *
 */
brightness_t Nixie_Tube::set_brightness(brightness_t bri) {
  this->_brightness = bri;

  this->end_effect();
  for (int i=0; i < this->element_n; i++) {
    this->element[i].set_max_brightness(bri);
    if ( this->element[i].brightness() ) {
      this->element[i].set_brightness(bri);
    }
  } // for

  return this->_brightness;
} // Nixie_Tube::set_brightness()

/**
 *
 */
void Nixie_Tube::end_effect() {
  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      e.second->end();
    }
  }
} // Nixie_Tube::end_effect()

/**
 *
 */
boolean Nixie_Tube::effect_is_active() {
  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      return true;
    }
  }
  return false;
} // Nixie_Tube::effect_is_active()

//============================================================================
/**
 *
 */
void Nixie_Tube::ef_only(int el_i) {
  for (int i=0; i < this->element_n; i++) {
    if ( i == el_i ) {
      this->element[i].set_brightness_to_max();
    } else {
      this->element[i].set_brightness(0);
    }
  } // for(i)
} // Nixie_Tube::ef_only()

/**
 *
 */
void Nixie_Tube::ef_xfade(int el_src, int el_dst,
                             unsigned long ms, unsigned long start_ms) {
  this->end_effect();

  if ( el_src != el_dst ) {
    this->ef["fadeout"]->start(el_src, ms, start_ms);
  }

  this->ef["fadein"]->start(el_dst, ms, start_ms);
} // Nixie_Tube::ef_xfade()
