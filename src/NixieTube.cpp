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
  this->_ef1 = (NixieEffect *)NULL;
  this->_ef2 = (NixieEffect *)NULL;

  this->ef_fadein = new NixieEffectFadeIn(this->element, this->element_n);
  this->ef_fadeout = new NixieEffectFadeOut(this->element, this->element_n);
  this->ef_fog = new NixieEffectFog(this->element, this->element_n);
  this->ef_shuffle = new NixieEffectShuffle(this->element, this->element_n);
  this->ef_blink = new NixieEffectBlink(this->element, this->element_n);
} // NixieTube::setup()

/**
 *
 */
void NixieTube::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  
  if ( this->_ef1 != (NixieEffect *)NULL ) {
    if ( this->_ef1->is_active() ) {
      this->_ef1->loop(cur_ms);
    } else {
      //delete this->_ef1;
      this->_ef1 = (NixieEffect *)NULL;
    }
  }

  if ( this->_ef2 != (NixieEffect *)NULL ) {
    if ( this->_ef2->is_active() ) {
      this->_ef2->loop(cur_ms);
    } else {
      //delete this->_ef2;
      this->_ef2 = (NixieEffect *)NULL;
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
  if ( this->_ef1 ) {
    if ( this->_ef1->is_active() ) {
      this->_ef1->end();
    }
    //delete this->_ef1;
    this->_ef1 = NULL;
  }

  if ( this->_ef2 ) {
    if ( this->_ef2->is_active() ) {
      this->_ef2->end();
    }
    //delete this->_ef2;
    this->_ef2 = NULL;
  }  
} // NixieTube::end_effect()

/**
 *
 */
boolean NixieTube::effect_is_active() {
  if ( this->_ef1 ) {
    if ( this->_ef1->is_active() ) {
      return true;
    }
    this->_ef1 = NULL;
  }
  if ( this->_ef2 ) {
    if ( this->_ef2->is_active() ) {
      return true;
    }
    this->_ef1 = NULL;
  }
  return false;
} // NixieTube::effect_is_active()

//============================================================================
/**
 *
 */
void NixieTube::effect_one(int el_i) {
  for (int i=0; i < this->element_n; i++) {
    if ( i == el_i ) {
      this->element[i].set_brightness_to_max();
    } else {
      this->element[i].set_brightness(0);
    }
  } // for(i)
} // NixieTube::effect_one()

/**
 *
 */
void NixieTube::effect_fadein(int el_i,
                              unsigned long ms, unsigned long start_ms) {
  this->_ef1 = this->ef_fadein;
  this->ef_fadein->start(el_i, ms, start_ms);
} // NixieTube::effect_fadein()

/**
 *
 */
void NixieTube::effect_fadeout(int el_i,
                               unsigned long ms, unsigned long start_ms) {
  this->_ef1 = this->ef_fadeout;
  this->_ef1->start(el_i, ms, start_ms);
} // NixieTube::effect_fadeout()

/**
 *
 */
void NixieTube::effect_xfade(int el_src, int el_dst,
                             unsigned long ms, unsigned long start_ms) {
  this->end_effect();

  if ( el_src != el_dst ) {
    this->_ef1 = this->ef_fadeout;
    this->_ef1->start(el_src, ms, start_ms);
  } else {
    this->_ef1 = NULL;
  }

  this->_ef2 = this->ef_fadein;
  this->_ef2->start(el_dst, ms, start_ms);
} // NixieTube::effect_xfade()

/**
 *
 */
void NixieTube::effect_fog(int el_i, unsigned long ms, unsigned long start_ms) {
  this->_ef1 = this->ef_fog;
  this->_ef1->start(el_i, ms, start_ms);
} // NixieTube::effect_fadeout()

/**
 *
 */
void NixieTube::effect_shuffle(int el_i, int n,
                               unsigned long ms, unsigned long start_ms) {
  this->_ef1 = this->ef_shuffle;
  this->_ef1->start(el_i, n, ms, start_ms);
} // NixieTube::effect_shuffle()

/**
 *
 */
void NixieTube::effect_blink(unsigned long ms, unsigned long start_ms) {
  this->_ef1 = this->ef_blink;
  this->_ef1->start(ms, start_ms);
  
} // NixieTube::effect_blink()
