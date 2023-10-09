/*
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NixieEffect.h"
#include "NixieTubeArray.h"

extern NixieTubeArray *nta;
// XXX nta->brightness をグローバル化?

//============================================================================
// class NixieEffect
//----------------------------------------------------------------------------
NixieEffect::NixieEffect(effect_id_t eid,
                         NixieElement *element,
                         unsigned long el_n) {
  this->_id = eid;
  this->_el = element;
  this->_el_n = el_n;
}

NixieEffect::~NixieEffect() {}

void NixieEffect::start(unsigned long start_ms,
                        unsigned long tick_ms) {
  this->_start_ms = start_ms;
  this->_tick_ms = tick_ms;
  (void)this->tick(this->_start_ms);
  this->_active = true;
}

void NixieEffect::start(unsigned long start_ms,
                        unsigned long tick_ms,
                        int el) {
  this->start(start_ms, tick_ms);
}

void NixieEffect::start(unsigned long start_ms,
                        unsigned long tick_ms,
                        int el1,
                        int el2) {
  this->start(start_ms, tick_ms);
}

void NixieEffect::loop(unsigned long cur_ms) {
  if ( ! tick(cur_ms) ) {
    return;
  }
} // NixieEffect::loop()

void NixieEffect::end() {
  this->_active = false;
} // NixieEffect::end()

effect_id_t NixieEffect::get_id() {
  return this->_id;
} // NixieEffect::get_id()

boolean NixieEffect::is_active() {
  return this->_active;
} // NixieEffect::is_active()

boolean NixieEffect::tick(unsigned long cur_ms) {
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
// class NixieEffectOnly
//----------------------------------------------------------------------------
NixieEffectOnly::NixieEffectOnly(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_ONLY, el, el_n) {
}

void NixieEffectOnly::start(unsigned long start_ms,
                            unsigned long tick_ms,
                            int el_i,
                            int bl) {
  NixieEffect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  this->_bl = bl;

  for (int i=0; i < this->_el_n; i++) {
    if (i == this->_el_i) {
      this->_el[this->_el_i].set_brightness(this->_bl);
    } else {
      this->_el[this->_el_i].set_brightness(0);
    }
  } // NixieEffectOnly::start()
}

void NixieEffectOnly::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  // do nothing
  return;
} // NixieEffectOnly::loop()
//============================================================================
// class NixieEffectFadeIn
//----------------------------------------------------------------------------
NixieEffectFadeIn::NixieEffectFadeIn(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_FADEIN, el, el_n) {
}

void NixieEffectFadeIn::start(unsigned long start_ms,
                              unsigned long tick_ms,
                              int el_i) {
  NixieEffect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  //this->_el[this->_el_i].set_brightness(0);
} // NixieEffectFadeIn::start()

void NixieEffectFadeIn::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e = &(this->_el[this->_el_i]); // 重要！ポインタ渡し
  uint8_t bl = e->get_brightness();
  if ( bl < nta->brightness ) {
    e->inc_brightness();
  } else {
    this->end();
  }
} // NixieEffectFadeIn::loop()
//============================================================================
// class NixieEffectFadeOut
//----------------------------------------------------------------------------
NixieEffectFadeOut::NixieEffectFadeOut(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_FADEOUT, el, el_n) {
}

void NixieEffectFadeOut::start(unsigned long start_ms,
                               unsigned long tick_ms,
                               int el_i) {
  NixieEffect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  //this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
}

void NixieEffectFadeOut::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e = &(this->_el[this->_el_i]);
  uint8_t bl = e->get_brightness();
  if ( bl > 0 ) {
    e->dec_brightness();
  } else {
    this->end();
  }
} // NixieEffectFadeOut::loop()

//============================================================================
// class NixieEffectXFade
//----------------------------------------------------------------------------
NixieEffectXFade::NixieEffectXFade(NixieElement *el, unsigned long el_n)
  : NixieEffect::NixieEffect(NIXIE_EFFECT_XFADE, el, el_n) {
}

void NixieEffectXFade::start(unsigned long start_ms,
                             unsigned long tick_ms,
                             int el_i_in,
                             int el_i_out) {
  NixieEffect::start(start_ms, tick_ms);

  // Serial.println("NixieEffectXFade::start> ");
  this->_el_i_in  = el_i_in;
  this->_el_i_out = el_i_out;
  for (int e = 0; e < this->_el_n; e++) {
    NixieElement *el = &(this->_el[e]);
    if ( e == this->_el_i_out ) {
      el->set_brightness(nta->brightness);
    } else {
      el->set_brightness(0);
    }
  } // for(e)
} // NixieEffectXFade::start()

void NixieEffectXFade::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e_in  = &(this->_el[this->_el_i_in]);
  NixieElement *e_out = &(this->_el[this->_el_i_out]);
  uint8_t bl_in  = e_in->get_brightness();
  uint8_t bl_out = e_out->get_brightness();
  int end_count = 0;
  if ( bl_in < nta->brightness ) {
    e_in->inc_brightness();
  } else {
    end_count++;
  }
  if ( bl_out > 0 ) {
    e_out->dec_brightness();
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
NixieEffectShuffle::NixieEffectShuffle(NixieElement *el, unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_SHUFFLE, el, el_n) {
}

void NixieEffectShuffle::start(unsigned long start_ms,
                               unsigned long tick_ms,
                               int n,
                               int el_i) {
  NixieEffect::start(start_ms, tick_ms);

  this->_n = n;
  this->_el_i = el_i;

  for (int e=0; e < this->_el_n; e++) {
    this->_el[e].set_brightness(0);
  }
} // NixieEffectShuffle::start()

void NixieEffectShuffle::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  this->_el[this->_el_random].set_brightness(0);

  if ( this->_tick >= this->_n ) {
    this->end();
    return;
  }
  
  this->_el_random = random(this->_el_n);
  this->_el[this->_el_random].set_brightness(nta->brightness);

} // NixieEffectShuffle::loop()

void NixieEffectShuffle::end() {
  NixieEffect::end();

  this->_el[this->_el_i].set_brightness(nta->brightness);

} // NixieEffectShuffle::end()

//============================================================================
// class NixieEffectBlink
//----------------------------------------------------------------------------
NixieEffectBlink::NixieEffectBlink(NixieElement *el, unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_BLINK, el, el_n) {
} // NixieEffectBlink::NixieEffectBlink()

void NixieEffectBlink::start(unsigned long start_ms,
                             unsigned long tick_ms) {
  NixieEffect::start(start_ms, tick_ms);

  this->_onoff = true;

  for (int i=0; i < this->_el_n; i++) {
    this->_brightness[i] = this->_el[i].get_brightness();
  } // for(i)
} // NixieEffectBlink::start()

void NixieEffectBlink::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  if ( this->_onoff ) {
    this->_onoff = false;
    for (int i=0; i < this->_el_n; i++) {
      this->_el[i].set_brightness(0);
    } // for(i)
    return;
  }

  // this->_onoff == false
  this->_onoff = true;
  for (int i=0; i < this->_el_n; i++) {
    this->_el[i].set_brightness(this->_brightness[i]);
  } // for(i)
} // NixieEffectBlink::loop()

void NixieEffectBlink::end() {
  NixieEffect::end();

  for (int e=0; e < this->_el_n; e++) {
    this->_el[e].set_brightness(this->_brightness[e]);
  } // for(e)
} // NixieEffectBlink::end()

//============================================================================
// class NixieEffectRandomOnOff
//----------------------------------------------------------------------------
NixieEffectRandomOnOff::NixieEffectRandomOnOff(NixieElement *el,
                                               unsigned long el_n)
  : NixieEffect(NIXIE_EFFECT_RANDOM_ONOFF, el, el_n) {
} // NixieEffectRandomOnOff::NixieEffectRandomOnOff()

void NixieEffectRandomOnOff::start(unsigned long start_ms,
                              unsigned long tick_ms,
                              int el_i) {
  NixieEffect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
} // NixieEffectRandomOnOff::start()

void NixieEffectRandomOnOff::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e = &(this->_el[this->_el_i]); // 重要！ポインタ渡し
  if (random(10) > 5) {
    e->set_brightness(BRIGHTNESS_RESOLUTION);
  } else {
    e->set_brightness(0);    
  }
} // NixieEffectRandomOnOff::loop()

void NixieEffectRandomOnOff::end() {
  NixieEffect::end();

  this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
} // NixieEffectRandomOnOff::end()
