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
NixieEffect::NixieEffect(effect_id_t eid,
                         NixieElement *element,
                         unsigned long el_n) {
  this->_id = eid;
  this->_el = element;
  this->_el_n = el_n;
}

/**
 *
 */
NixieEffect::~NixieEffect() {}

/**
 *
 */
void NixieEffect::start(unsigned long tick_ms, unsigned long start_ms) {
  this->_tick_ms = tick_ms;
  if ( start_ms == 0 ) {
    this->_start_ms = millis();
  } else {
    this->_start_ms = start_ms;
  }

  (void)this->tick(this->_start_ms);
  this->_active = true;
}

/**
 *
 */
void NixieEffect::start(int el, unsigned long tick_ms, unsigned long start_ms) {
  this->start(tick_ms, start_ms);
}

/**
 *
 */
void NixieEffect::start(int el1, int el2,
                        unsigned long tick_ms, unsigned long start_ms) {
  this->start(tick_ms, start_ms);
}

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
} // NixieEffect::loop()

/**
 *
 */
void NixieEffect::end() {
  this->_active = false;
} // NixieEffect::end()

/**
 *
 */
effect_id_t NixieEffect::get_id() {
  return this->_id;
} // NixieEffect::get_id()

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
  if ( this->_tick_ms == 0 ) {
    return false;
  }
  this->_prev_tick = this->_tick;
  this->_tick = (cur_ms - this->_start_ms) / this->_tick_ms;
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
NixieEffectFadeIn::NixieEffectFadeIn(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_FADEIN, el, el_n) {
}

/**
 *
 */
void NixieEffectFadeIn::start(int el_i,
                              unsigned long tick_ms, unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_el_i = el_i;
} // NixieEffectFadeIn::start()

/**
 *
 */
void NixieEffectFadeIn::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  if ( this->_el_i < this->_el_n ) {
    NixieElement *e = &(this->_el[this->_el_i]);
    if ( e->brightness() < e->max_brightness() ) {
      e->inc_brightness();
    } else {
      this->end();
    }
  }
} // NixieEffectFadeIn::loop()

/**
 *
 */
void NixieEffectFadeIn::end() {
  if ( this->_el_i < this->_el_n ) {
    NixieElement *e = &(this->_el[this->_el_i]);
    e->set_brightness_to_max();
  }
  NixieEffect::end();
} // NixieEffectFadeIn::end()

//============================================================================
// class NixieEffectFadeOut
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectFadeOut::NixieEffectFadeOut(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_FADEOUT, el, el_n) {
}

/**
 *
 */
void NixieEffectFadeOut::start(int el_i,
                               unsigned long tick_ms, unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_el_i = el_i;
}

/**
 *
 */
void NixieEffectFadeOut::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  if ( this->_el_i < this->_el_n ) {
    NixieElement *e = &(this->_el[this->_el_i]);
    if ( e->brightness() > 0 ) {
      e->dec_brightness();
    } else {
      this->end();
    }
  }
} // NixieEffectFadeOut::loop()

/**
 *
 */
void NixieEffectFadeOut::end() {
  if ( this->_el_i < this->_el_n ) {
    NixieElement *e = &(this->_el[this->_el_i]);
    e->set_brightness(0);
  }
  NixieEffect::end();
} // NixieEffectFadeOut::end()

//============================================================================
// class NixieEffectXFade
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectXFade::NixieEffectXFade(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_XFADE, el, el_n) {
}

/**
 *
 */
void NixieEffectXFade::start(int el_i_src, int el_i_dst,
                             unsigned long tick_ms, unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_el_i_src  = el_i_src;
  this->_el_i_dst = el_i_dst;
#if 0
  for (int i = 0; i < this->_el_n; i++) {
    NixieElement *el = &(this->_el[i]);
    if ( i == this->_el_i_src ) {
      el->set_brightness_to_max();
    } else {
      el->set_brightness(0);
    }
  } // for(i)
#endif
} // NixieEffectXFade::start()

/**
 *
 */
void NixieEffectXFade::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  int end_count = 0;

  if ( this->_el_i_src < this->_el_n ) {
    NixieElement *e_src = &(this->_el[this->_el_i_src]);
    if ( e_src->brightness() > 0 ) {
      e_src->dec_brightness();
    } else {
      end_count++;
    }
  } else {
    end_count++;
  }
  
  if ( this->_el_i_dst < this->_el_n ) {
    NixieElement *e_dst  = &(this->_el[this->_el_i_dst]);
    if ( e_dst->brightness() < e_dst->max_brightness() ) {
      e_dst->inc_brightness();
    } else {
      end_count++;
    }
  } else {
    end_count++;
  }
  
  if ( end_count >= 2 ) {
    this->end();
  }
} // NixieEffectXFade::loop()

//============================================================================
// class NixieEffectShuffle
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectShuffle::NixieEffectShuffle(NixieElement *el, unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_SHUFFLE, el, el_n) {
}

/**
 *
 */
void NixieEffectShuffle::start(int n, int el_i,
                               unsigned long tick_ms, unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_n = n;
  this->_el_i = el_i;

  for (int e=0; e < this->_el_n; e++) {
    this->_el[e].set_brightness(0);
  }
} // NixieEffectShuffle::start()

/**
 *
 */
void NixieEffectShuffle::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  this->_el[this->_el_random].set_brightness(0);

  if ( this->_tick >= this->_n ) {
    this->end();
    return;
  }
  
  this->_el_random = random(this->_el_n);
  this->_el[this->_el_random].set_brightness_to_max();

} // NixieEffectShuffle::loop()

/**
 *
 */
void NixieEffectShuffle::end() {
  this->_el[this->_el_i].set_brightness_to_max();

  NixieEffect::end();
} // NixieEffectShuffle::end()

//============================================================================
// class NixieEffectBlink
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectBlink::NixieEffectBlink(NixieElement *el, unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_BLINK, el, el_n) {
} // NixieEffectBlink::NixieEffectBlink()

/**
 *
 */
void NixieEffectBlink::start(unsigned long tick_ms, unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_is_on = true;

  for (int i=0; i < this->_el_n; i++) {
    this->_brightness[i] = this->_el[i].brightness();
  } // for(i)
} // NixieEffectBlink::start()

/**
 *
 */
void NixieEffectBlink::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  for (int i=0; i < this->_el_n; i++) {
    if ( this->_is_on ) {
      this->_el[i].set_brightness(0);
    } else {
      this->_el[i].set_brightness(this->_brightness[i]);
    }
  } // for(i)
  this->_is_on = ! this->_is_on;
} // NixieEffectBlink::loop()

/**
 *
 */
void NixieEffectBlink::end() {
  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(this->_brightness[i]);
  } // for(i)
  NixieEffect::end();
} // NixieEffectBlink::end()

//============================================================================
// class NixieEffectRandomOnOff
//----------------------------------------------------------------------------
/**
 *
 */
NixieEffectRandomOnOff::NixieEffectRandomOnOff(NixieElement *el,
                                               unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_RANDOM_ONOFF, el, el_n) {
} // NixieEffectRandomOnOff::NixieEffectRandomOnOff()

/**
 *
 */
void NixieEffectRandomOnOff::start(int el_i,
                                   unsigned long tick_ms,
                                   unsigned long start_ms) {
  NixieEffect::start(tick_ms, start_ms);

  this->_el_i = el_i;
  this->_el[this->_el_i].set_brightness_to_max();
} // NixieEffectRandomOnOff::start()

/**
 *
 */
void NixieEffectRandomOnOff::loop(unsigned long cur_ms) {
  if ( cur_ms == 0 ) {
    cur_ms = millis();
  }
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e = &(this->_el[this->_el_i]); // 重要！ポインタ渡し
  if (random(10) > 5) {
    e->set_brightness_to_max();
  } else {
    e->set_brightness(0);    
  }
} // NixieEffectRandomOnOff::loop()

/**
 *
 */
void NixieEffectRandomOnOff::end() {
  this->_el[this->_el_i].set_brightness_to_max();
  NixieEffect::end();
} // NixieEffectRandomOnOff::end()
