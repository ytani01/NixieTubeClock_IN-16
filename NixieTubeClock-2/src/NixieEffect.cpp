/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NixieEffect.h"

//============================================================================
// class NixieEffect
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffect::NixieEffect(NixieElement *element, unsigned long el_n) {
  this->_el = element;
  this->_el_n = el_n;
}

/**
 *
 */
NixieEffect::~NixieEffect() {}

/** virtual
 *
 */
void NixieEffect::start(unsigned long ms, unsigned long start_ms) {
  this->_ms = ms;

  if ( start_ms == 0 ) {
    this->_start_ms = millis();
  } else {
    this->_start_ms = start_ms;
  }

  (void)this->tick(this->_start_ms);
  this->_active = true;
} // NixieEffect::start()

/**
 *
 */
void NixieEffect::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! tick(cur_ms) ) {
    return;
  }

  this->do_loop();
} // NixieEffect::loop()

/** virtual
 *
 */
void NixieEffect::do_loop() {
  
} // NixieEffect::do_loop()

/**
 *
 */
void NixieEffect::end() {
  this->_active = false;
} // NixieEffect::end()

/**
 *
 */
boolean NixieEffect::is_active() {
  return this->_active;
} // NixieEffect::is_active()

/**
 *
 */
boolean NixieEffect::tick(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( this->_ms == 0 ) {
    return false;
  }

  this->_prev_tick = this->_tick;
  this->_tick = (cur_ms - this->_start_ms) / this->_ms;
  if ( this->_tick == this->_prev_tick ) {
    return false;
  }
  return true;
} // NixieEffect::tick()

//============================================================================
// class NixieEffectFadeIn
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectFadeIn::NixieEffectFadeIn(int el_i,
                                     NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(el, el_n)
{
  if ( el_i < el_n ) {
    this->_el1 = &(el[el_i]);
    this->_el1->set_brightness(0);
  } else {
    this->_el1 = NULL;
  }
} // NixieEffectFadeIn::NixieEffectFadeIn()

/** virtual
 *
 */
void NixieEffectFadeIn::do_loop() {
  if ( this->_el1 ) {
    if ( this->_el1->brightness() < this->_el1->max_brightness() ) {
      this->_el1->inc_brightness();
    } else {
      this->end();
    }
  }
} // NixieEffectFadeIn::do_loop()

/**
 *
 */
void NixieEffectFadeIn::end() {
  if ( this->_el1 ) {
    this->_el1->set_brightness_to_max();
  }
  NixieEffect::end();
} // NixieEffectFadeIn::end()

//============================================================================
// class NixieEffectFadeOut
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectFadeOut::NixieEffectFadeOut(int el_i,
                                       NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(el, el_n)
{
  if ( el_i < el_n ) {
    this->_el1 = &(el[el_i]);
  } else {
    this->_el1 = NULL;
  }
}

/** virtual
 *
 */
void NixieEffectFadeOut::do_loop() {
  if ( this->_el1 ) {
    if ( this->_el1->brightness() > 0 ) {
      this->_el1->dec_brightness();
    } else {
      this->end();
    }
  }
} // NixieEffectFadeOut::do_loop()

/** virtual
 *
 */
void NixieEffectFadeOut::end() {
  if ( this->_el1 ) {
    this->_el1->set_brightness(0);
  }
  NixieEffect::end();
} // NixieEffectFadeOut::end()

//============================================================================
// class NixieEffectFog
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectFog::NixieEffectFog(int el_i,
                               NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(el, el_n)
{
  this->_el_i = el_i;
  
  this->_fog_up = true;
}

/** virtual
 *
 */
void NixieEffectFog::do_loop() {
  int count = 0;

  if ( _fog_up ) {
    for (int i=0; i < this->_el_n; i++) {
      if ( this->_el[i].brightness() < this->_el[i].max_brightness() ) {
        this->_el[i].inc_brightness();
      }
      if ( this->_el[i].brightness() >= this->_el[i].max_brightness() ) {
        count++;
      }
    } // for(i)
    if ( count >= this->_el_n ) {
      _fog_up = false;
    }
  } else {
    for (int i=0; i < this->_el_n; i++) {
      if ( i != this->_el_i ) {
        if ( this->_el[i].brightness() > 0 ) {
          this->_el[i].dec_brightness();
        }
        if ( this->_el[i].brightness() == 0 ) {
          count++;
        }
      }
    } // for(i)
    if ( this->_el_i < this->_el_n ) {
      if ( count >= this->_el_n - 1) {
        this->end();
        return;
      }
    } else {
      if ( count >= this->_el_n ) {
        this->end();
        return;
      }
    }
  } // if
} // NixieEffectFog::do_loop()

/** virtual
 *
 */
void NixieEffectFog::end() {
  for (int i=0; i < this->_el_n; i++) {
    if ( i == this->_el_i ) {
      this->_el[i].set_brightness_to_max();
    } else {
      this->_el[i].set_brightness(0);
    }
  }
  NixieEffect::end();
} // NixieEffectFog::end()

//============================================================================
// class NixieEffectShuffle
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectShuffle::NixieEffectShuffle(int el_i, int n,
                                       NixieElement *el, unsigned long el_n)
  : NixieEffect(el, el_n)
{
  this->_el_i = el_i;
  this->_n = n;

  this->_el_random = random(this->_el_n);
}

/** virtual
 *
 */
void NixieEffectShuffle::start(unsigned long ms, unsigned long start_ms) {
  NixieEffect::start(ms, start_ms);

  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(0);
  }
} // NixieEffectShuffle::start()

/** virtual
 *
 */
void NixieEffectShuffle::do_loop() {
  this->_el[this->_el_random].set_brightness(0);

  if ( this->_tick >= this->_n ) {
    this->end();
    return;
  }
  
  this->_el_random = random(this->_el_n);
  this->_el[this->_el_random].set_brightness_to_max();
} // NixieEffectShuffle::do_loop()

/** virtual
 *
 */
void NixieEffectShuffle::end() {
  for (int i=0; i < this->_el_n; i++) {
    if ( i == this->_el_i ) {
      this->_el[i].set_brightness_to_max();
    } else {
      this->_el[i].set_brightness(0);
    }
  }
  NixieEffect::end();
} // NixieEffectShuffle::end()

//============================================================================
// class NixieEffectBlink
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectBlink::NixieEffectBlink(NixieElement *el, unsigned long el_n)
  : NixieEffect(el, el_n)
{
} // NixieEffectBlink::NixieEffectBlink()

/** virtual
 *
 */
void NixieEffectBlink::start(unsigned long ms, unsigned long start_ms) {
  NixieEffect::start(ms, start_ms);

  this->_is_on = true;

  for (int i=0; i < this->_el_n; i++) {
    this->_brightness[i] = this->_el[i].brightness();
  } // for(i)
} // NixieEffectBlink::start()

/** virtual
 *
 */
void NixieEffectBlink::do_loop() {
  for (int i=0; i < this->_el_n; i++) {
    if ( this->_is_on ) {
      this->_el[i].set_brightness(0);
    } else {
      this->_el[i].set_brightness(this->_brightness[i]);
    }
  } // for(i)
  this->_is_on = ! this->_is_on;
} // NixieEffectBlink::do_loop()

/** virtual
 *
 */
void NixieEffectBlink::end() {
  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(this->_brightness[i]);
  } // for(i)
  NixieEffect::end();
} // NixieEffectBlink::end()
