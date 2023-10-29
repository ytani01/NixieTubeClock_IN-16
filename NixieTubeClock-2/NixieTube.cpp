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
NixieEffect *NixieTube::init_effect(effect_id_t eid) {
  switch (eid) {
  case NIXIE_EFFECT_FADEIN:
    return new NixieEffectFadeIn(this->element, this->element_n);
  case NIXIE_EFFECT_FADEOUT:
    return new NixieEffectFadeOut(this->element, this->element_n);
  case NIXIE_EFFECT_SHUFFLE:
    return new NixieEffectShuffle(this->element, this->element_n);
  case NIXIE_EFFECT_BLINK:
    return new NixieEffectBlink(this->element, this->element_n);
  case NIXIE_EFFECT_RANDOM_ONOFF:
    return new NixieEffectRandomOnOff(this->element, this->element_n);
  default:
    log_e("ERROR: eid = %d", eid);
    return (NixieEffect *)NULL;
  }
} // NixieTube::init_effect()

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

/**
 *
 */
void NixieTube::one(int el_i) {
  for (int i=0; i < this->element_n; i++) {
    if ( i == el_i ) {
      this->element[i].set_brightness_to_max();
    } else {
      this->element[i].set_brightness(0);
    }
  } // for(i)
} // NixieTube::one()

/**
 *
 */
void NixieTube::fadein_start(int el_i,
                             unsigned long tick_ms, unsigned long start_ms) {
  this->_ef1 = this->init_effect(NIXIE_EFFECT_FADEIN);
  this->_ef1->start(el_i, tick_ms, start_ms);
} // NixieTube::fadein_start()

/**
 *
 */
void NixieTube::fadeout_start(int el_i,
                              unsigned long tick_ms, unsigned long start_ms) {
  this->_ef1 = this->init_effect(NIXIE_EFFECT_FADEOUT);
  this->_ef1->start(el_i, tick_ms, start_ms);
} // NixieTube::fadeout_start()

/**
 *
 */
void NixieTube::xfade_start(int el_src, int el_dst,
                            unsigned long tick_ms, unsigned long start_ms) {
  this->end_effect();
  this->_ef1 = this->init_effect(NIXIE_EFFECT_FADEOUT);
  this->_ef1->start(el_src, tick_ms, start_ms);
  this->_ef2 = this->init_effect(NIXIE_EFFECT_FADEIN);
  this->_ef2->start(el_dst, tick_ms, start_ms);
} // NixieTube::xfade_start()

/**
 *
 */
void NixieTube::shuffle_start(int n, int el_i,
                              unsigned long tick_ms, unsigned long start_ms) {
  this->_ef1 = this->init_effect(NIXIE_EFFECT_SHUFFLE);
  this->_ef1->start(n, el_i, tick_ms, start_ms);
} // NixieTube::shuffle_start()

/**
 *
 */
void NixieTube::blink_start(unsigned long tick_ms, unsigned long start_ms) {
  this->_ef1 = this->init_effect(NIXIE_EFFECT_BLINK);
  this->_ef1->start(tick_ms, start_ms);
} // NixieTube::blink_start()

/**
 *
 */
void NixieTube::randomOnOff_start(int el_i,
                                  unsigned long tick_ms,
                                  unsigned long start_ms) {
  this->_ef1 = this->init_effect(NIXIE_EFFECT_RANDOM_ONOFF);
  this->_ef1->start(el_i, tick_ms, start_ms);
} // NixieTube::randomOnOff()
