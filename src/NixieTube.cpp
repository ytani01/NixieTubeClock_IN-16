/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NixieTube.h"

/**
 *
 */
NixieTube::NixieTube() {
} // constructor

/**
 *
 */
void NixieTube::setup(int element_n, uint8_t *pin) {
  this->element_n = element_n;
  this->element = new NixieElement[this->element_n];
  
  for (int d=0; d < element_n; d++) {
    this->element[d].setup(pin[d]);
  } // for (d)

  this->ef["fadein"] = new NixieEffectFadeIn(this->element, this->element_n);
  this->ef["fadeout"] = new NixieEffectFadeOut(this->element, this->element_n);
  this->ef["fog"] = new NixieEffectFog(this->element, this->element_n);
  this->ef["shuffle"] = new NixieEffectShuffle(this->element, this->element_n);
  this->ef["blink"] = new NixieEffectBlink(this->element, this->element_n);
} // NixieTube::setup()

/**
 *
 */
void NixieTube::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }

  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      e.second->loop(cur_ms);
    }
  }  
} // NixieTube::loop()

/**
 *
 */
brightness_t NixieTube::brightness() {
  return this->_brightness;
} // NixieTube::brightness()

/**
 *
 */
brightness_t NixieTube::set_brightness(brightness_t bri) {
  this->_brightness = bri;

  this->end_effect();
  for (int i=0; i < this->element_n; i++) {
    this->element[i].set_max_brightness(bri);
    if ( this->element[i].brightness() ) {
      this->element[i].set_brightness(bri);
    }
  } // for

  return this->_brightness;
} // NixieTube::set_brightness()

/**
 *
 */
void NixieTube::end_effect() {
  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      e.second->end();
    }
  }
} // NixieTube::end_effect()

/**
 *
 */
boolean NixieTube::effect_is_active() {
  for (auto e: this->ef) {
    if ( e.second->is_active() ) {
      return true;
    }
  }
  return false;
} // NixieTube::effect_is_active()

//============================================================================
/**
 *
 */
void NixieTube::ef_only(int el_i) {
  for (int i=0; i < this->element_n; i++) {
    if ( i == el_i ) {
      this->element[i].set_brightness_to_max();
    } else {
      this->element[i].set_brightness(0);
    }
  } // for(i)
} // NixieTube::ef_only()

/**
 *
 */
void NixieTube::ef_xfade(int el_src, int el_dst,
                             unsigned long ms, unsigned long start_ms) {
  this->end_effect();

  if ( el_src != el_dst ) {
    this->ef["fadeout"]->start(el_src, ms, start_ms);
  }

  this->ef["fadein"]->start(el_dst, ms, start_ms);
} // NixieTube::ef_xfade()
