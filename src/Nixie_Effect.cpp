/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Nixie_Effect.h"

//============================================================================
// class Nixie_Effect
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_Effect::Nixie_Effect(Nixie_Element *element, unsigned long el_n) {
  this->_el = element;
  this->_el_n = el_n;
}

/**
 *
 */
Nixie_Effect::~Nixie_Effect() {}

/** virtual
 *
 */
void Nixie_Effect::start(unsigned long ms, unsigned long start_ms) {
  this->_ms = ms;

  if ( start_ms == 0 ) {
    this->_start_ms = millis();
  } else {
    this->_start_ms = start_ms;
  }

  (void)this->tick(this->_start_ms);
  this->_active = true;
} // Nixie_Effect::start()

/** virtual
 *
 */
void Nixie_Effect::start(int el_i, unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(ms, start_ms);

  this->_el_i = el_i;

  this->_el1 = NULL;
  if ( el_i < this->_el_n ) {
    this->_el1 = &(this->_el[el_i]);
  } else {
    this->_el1 = NULL;
  }
} // Nixie_Effect::start()

/** virtual
 *
 */
void Nixie_Effect::start(int el_i, int n,
                        unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(el_i, ms, start_ms);

  this->_n = n;
} // Nixie_Effect::start()

/**
 *
 */
void Nixie_Effect::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! tick(cur_ms) ) {
    return;
  }

  this->do_loop();
} // Nixie_Effect::loop()

/** virtual
 *
 */
void Nixie_Effect::do_loop() {
  
} // Nixie_Effect::do_loop()

/**
 *
 */
void Nixie_Effect::end() {
  this->_active = false;
} // Nixie_Effect::end()

/**
 *
 */
boolean Nixie_Effect::is_active() {
  return this->_active;
} // Nixie_Effect::is_active()

/**
 *
 */
boolean Nixie_Effect::tick(unsigned long cur_ms) {
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
} // Nixie_Effect::tick()

//============================================================================
// class Nixie_EffectFadeIn
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_EffectFadeIn::Nixie_EffectFadeIn(Nixie_Element *element,
                                     unsigned long el_n)
  : Nixie_Effect(element, el_n) {
} // constructor

/** virtual
 *
 */
void Nixie_EffectFadeIn::start(int el_i,
                              unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(el_i, ms, start_ms);

  if ( this->_el1 ) {
    this->_el1->set_brightness(0);
  }
} // Nixie_EffectFadein::start()

/** virtual
 *
 */
void Nixie_EffectFadeIn::do_loop() {
  if ( this->_el1 ) {
    if ( this->_el1->brightness() < this->_el1->max_brightness() ) {
      this->_el1->inc_brightness();
    } else {
      this->end();
    }
  }
} // Nixie_EffectFadeIn::do_loop()

/**
 *
 */
void Nixie_EffectFadeIn::end() {
  Nixie_Effect::end();

  if ( this->_el1 ) {
    this->_el1->set_brightness_to_max();
    this->_el1 = NULL;
  }
} // Nixie_EffectFadeIn::end()

//============================================================================
// class Nixie_EffectFadeOut
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_EffectFadeOut::Nixie_EffectFadeOut(Nixie_Element *element,
                                       unsigned long el_n)
  : Nixie_Effect(element, el_n) {
} // constructor

/** virtual
 *
 */
void Nixie_EffectFadeOut::start(int el_i,
                               unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(el_i, ms, start_ms);
} // Nixie_EffectFadeOut::start()

/** virtual
 *
 */
void Nixie_EffectFadeOut::do_loop() {
  if ( this->_el1 ) {
    if ( this->_el1->brightness() > 0 ) {
      this->_el1->dec_brightness();
    } else {
      this->end();
    }
  }
} // Nixie_EffectFadeOut::do_loop()

/** virtual
 *
 */
void Nixie_EffectFadeOut::end() {
  Nixie_Effect::end();

  if ( this->_el1 ) {
    this->_el1->set_brightness(0);
    this->_el1 = NULL;
  }
} // Nixie_EffectFadeOut::end()

//============================================================================
// class Nixie_EffectFog
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_EffectFog::Nixie_EffectFog(Nixie_Element *element, unsigned long el_n)
  : Nixie_Effect(element, el_n) {
} // constructor

/** virtual
 *
 */
void Nixie_EffectFog::start(int el_i,
                           unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(el_i, ms, start_ms);

  this->_fog_up = true;
} // Nixie_EffectFog::start()

/** virtual
 *
 */
void Nixie_EffectFog::do_loop() {
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
} // Nixie_EffectFog::do_loop()

/** virtual
 *
 */
void Nixie_EffectFog::end() {
  Nixie_Effect::end();

  for (int i=0; i < this->_el_n; i++) {
    if ( i == this->_el_i ) {
      this->_el[i].set_brightness_to_max();
    } else {
      this->_el[i].set_brightness(0);
    }
  }
  if ( this->_el1 ) {
    this->_el1 = NULL;
  }
} // Nixie_EffectFog::end()

//============================================================================
// class Nixie_EffectShuffle
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_EffectShuffle::Nixie_EffectShuffle(Nixie_Element *element,
                                       unsigned long el_n)
  : Nixie_Effect(element, el_n) {
} // constructor

/** virtual
 *
 */
void Nixie_EffectShuffle::start(int el_i, int n,
                               unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(el_i, ms, start_ms);

  this->_n = n;
  this->_el_random = 0;

  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(0);
  }
} // Nixie_EffectShuffle::start()

/** virtual
 *
 */
void Nixie_EffectShuffle::do_loop() {
  this->_el[this->_el_random].set_brightness(0);

  if ( this->_tick >= this->_n ) {
    this->end();
    return;
  }
  
  this->_el_random = random(this->_el_n);
  this->_el[this->_el_random].set_brightness_to_max();
} // Nixie_EffectShuffle::do_loop()

/** virtual
 *
 */
void Nixie_EffectShuffle::end() {
  Nixie_Effect::end();

  for (int i=0; i < this->_el_n; i++) {
    if ( i == this->_el_i ) {
      this->_el[i].set_brightness_to_max();
    } else {
      this->_el[i].set_brightness(0);
    }
  }
  if ( this->_el1 ) {
    this->_el1 = NULL;
  }
} // Nixie_EffectShuffle::end()

//============================================================================
// class Nixie_EffectBlink
//----------------------------------------------------------------------------
/**
 *
 */
Nixie_EffectBlink::Nixie_EffectBlink(Nixie_Element *element, unsigned long el_n)
  : Nixie_Effect(element, el_n) {
} // constructor

/** virtual
 *
 */
void Nixie_EffectBlink::start(unsigned long ms, unsigned long start_ms) {
  Nixie_Effect::start(ms, start_ms);

  this->_is_on = true;

  for (int i=0; i < this->_el_n; i++) {
    this->_brightness[i] = this->_el[i].brightness();
  } // for(i)
} // Nixie_EffectBlink::start()

/** virtual
 *
 */
void Nixie_EffectBlink::do_loop() {
  for (int i=0; i < this->_el_n; i++) {
    if ( this->_is_on ) {
      this->_el[i].set_brightness(0);
    } else {
      this->_el[i].set_brightness(this->_brightness[i]);
    }
  } // for(i)
  this->_is_on = ! this->_is_on;
} // Nixie_EffectBlink::do_loop()

/** virtual
 *
 */
void Nixie_EffectBlink::end() {
  Nixie_Effect::end();

  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(this->_brightness[i]);
  } // for(i)
} // Nixie_EffectBlink::end()
