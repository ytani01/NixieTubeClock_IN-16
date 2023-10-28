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
  this->_ef = (NixieEffect *)NULL;
} // NixieTube::setup()

/**
 *
 */
void NixieTube::loop(unsigned long cur_ms) {
  if ( this->_ef == (NixieEffect *)NULL ) {
    return;
  }
  if ( ! this->_ef->is_active() ) {
    delete this->_ef;
    this->_ef = (NixieEffect *)NULL;
    return;
  }

  // NixieEffect is active
  this->_ef->loop(cur_ms);

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
  if ( this->_ef != (NixieEffect *)NULL ) {
    if ( this->_ef->is_active() ) {
      this->_ef->end();
    }
    delete this->_ef;
    this->_ef = (NixieEffect *)NULL;
  }

  switch (eid) {
  case NIXIE_EFFECT_FADEIN:
    return new NixieEffectFadeIn(this->element, this->element_n);
  case NIXIE_EFFECT_FADEOUT:
    return new NixieEffectFadeOut(this->element, this->element_n);
  case NIXIE_EFFECT_XFADE:
    return new NixieEffectXFade(this->element, this->element_n);
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
  if ( this->_ef == (NixieEffect *)NULL ) {
    return;
  }
  if ( this->_ef->is_active() ) {
    this->_ef->end();
  }
  delete this->_ef;
  this->_ef = (NixieEffect *)NULL;
} // NixieTube::end_effect()

/**
 *
 */
boolean NixieTube::effect_is_active() {
  return this->_ef;
} // NixieTube::effect_is_active()

/**
 *
 */
void NixieTube::one(int element) {
  for (int e=0; e < this->element_n; e++) {
    if ( e == element ) {
      this->element[e].set_brightness_to_max();
    } else {
      this->element[e].set_brightness(0);
    }
  } // for(e)
} // NixieTube::one()

/**
 *
 */
void NixieTube::fadein_start(unsigned long start_ms,
                             unsigned long tick_ms,
                             int element) {
  this->_ef = this->init_effect(NIXIE_EFFECT_FADEIN);
  this->_ef->start(start_ms, tick_ms, element);
} // NixieTube::fadein_start()

/**
 *
 */
void NixieTube::fadeout_start(unsigned long start_ms,
                              unsigned long tick_ms,
                              int element) {
  this->_ef = this->init_effect(NIXIE_EFFECT_FADEOUT);
  this->_ef->start(start_ms, tick_ms, element);
} // NixieTube::fadeout_start()

/**
 *
 */
void NixieTube::xfade_start(unsigned long start_ms,
                            unsigned long tick_ms,
                            int el_in, int el_out) {
  this->_ef = this->init_effect(NIXIE_EFFECT_XFADE);

  this->_ef->start(start_ms, tick_ms, el_in, el_out);
} // NixieTube::xfade_start()

/**
 *
 */
void NixieTube::shuffle_start(unsigned long start_ms,
                              unsigned long tick_ms,
                              int n, int el) {
  this->_ef = this->init_effect(NIXIE_EFFECT_SHUFFLE);
  this->_ef->start(start_ms, tick_ms, n, el);
} // NixieTube::shuffle_start()

/**
 *
 */
void NixieTube::blink_start(unsigned long start_ms, unsigned long tick_ms) {
  log_d("");
  this->_ef = this->init_effect(NIXIE_EFFECT_BLINK);
  this->_ef->start(start_ms, tick_ms);
} // NixieTube::blink_start()

/**
 *
 */
void NixieTube::randomOnOff_start(unsigned long start_ms,
                                  unsigned long tick_ms,
                                  int el) {
  this->_ef = this->init_effect(NIXIE_EFFECT_RANDOM_ONOFF);
  this->_ef->start(start_ms, tick_ms, el);
} // NixieTube::randomOnOff()
