/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NixieTube.h"

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
      delete this->_ef1;
      this->_ef1 = (NixieEffect *)NULL;
    }
  }

  if ( this->_ef2 != (NixieEffect *)NULL ) {
    if ( this->_ef2->is_active() ) {
      this->_ef2->loop(cur_ms);
    } else {
      delete this->_ef2;
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
  if ( this->_ef1 != (NixieEffect *)NULL ) {
    if ( this->_ef1->is_active() ) {
      this->_ef1->end();
    } else {
      delete this->_ef1;
      this->_ef1 = (NixieEffect *)NULL;
    }
  }
  if ( this->_ef2 != (NixieEffect *)NULL ) {
    if ( this->_ef2->is_active() ) {
      this->_ef2->end();
    } else {
      delete this->_ef2;
      this->_ef2 = (NixieEffect *)NULL;
    }
  }  
} // NixieTube::end_effect()

/**
 *
 */
boolean NixieTube::effect_is_active() {
  return (this->_ef1 || this->_ef2);
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
  this->_ef1 = new NixieEffectFadeIn(el_i, this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_fadein()

/**
 *
 */
void NixieTube::effect_fadeout(int el_i,
                              unsigned long ms, unsigned long start_ms) {
  this->_ef1 = new NixieEffectFadeOut(el_i, this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_fadeout()

/**
 *
 */
void NixieTube::effect_xfade(int el_src, int el_dst,
                            unsigned long ms, unsigned long start_ms) {
  this->end_effect();

  if ( el_src != el_dst ) {
    this->_ef1 = new NixieEffectFadeOut(el_src, this->element, this->element_n);
    this->_ef1->start(ms, start_ms);
  }

  this->_ef2 = new NixieEffectFadeIn(el_dst, this->element, this->element_n);
  this->_ef2->start(ms, start_ms);
} // NixieTube::effect_xfade()

/**
 *
 */
void NixieTube::effect_fog(int el_i, unsigned long ms, unsigned long start_ms) {
  this->_ef1 = new NixieEffectFog(el_i, this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_fadeout()

/**
 *
 */
void NixieTube::effect_shuffle(int el_i, int n,
                              unsigned long ms, unsigned long start_ms) {
  this->_ef1 = new NixieEffectShuffle(el_i, n,
                                      this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_shuffle()

/**
 *
 */
void NixieTube::effect_blink(unsigned long ms, unsigned long start_ms) {
  this->_ef1 = new NixieEffectBlink(this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_blink()

/**
 *
 */
void NixieTube::effect_randomOnOff(int el_i,
                                  unsigned long ms,
                                  unsigned long start_ms) {
  this->_ef1 = new NixieEffectRandomOnOff(el_i, this->element, this->element_n);
  this->_ef1->start(ms, start_ms);
} // NixieTube::effect_randomOnOff()
