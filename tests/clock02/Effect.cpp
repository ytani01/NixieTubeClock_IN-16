/*
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Effect.h"
#include "NixieTubeArray.h"

extern NixieTubeArray *nta;
// XXX nta->brightness をグローバル化?

//============================================================================
// class Effect
//----------------------------------------------------------------------------
Effect::Effect(effect_id_t eid, NixieElement *element, unsigned long el_n) {
  this->_id = eid;
  this->_el = element;
  this->_el_n = el_n;
  // Serial.println("Effect::Effect> el_n=" + String(this->_el_n));
}

Effect::~Effect() {}

void Effect::start(unsigned long start_ms, unsigned long tick_ms) {
  this->_start_ms = start_ms;
  this->_tick_ms = tick_ms;
  (void)this->tick(this->_start_ms);
  this->_active = true;
  // Serial.println("Effect::start> start_ms=" + String(this->_start_ms));
}
void Effect::start(unsigned long start_ms, unsigned long tick_ms, int el) {
  this->start(start_ms, tick_ms);
}
void Effect::start(unsigned long start_ms, unsigned long tick_ms,
                   int el1, int el2) {
  this->start(start_ms, tick_ms);
}

void Effect::loop(unsigned long cur_ms) {
  if ( ! tick(cur_ms) ) {
    return;
  }
} // Effect::loop()

void Effect::end() {
  this->_active = false;
} // Effect::end()

effect_id_t Effect::get_id() {
  return this->_id;
} // Effect::get_id()

boolean Effect::is_active() {
  return this->_active;
} // Effect::is_active()

boolean Effect::tick(unsigned long cur_ms) {
  if ( this->_tick_ms == 0 ) {
    return false;
  }
  this->_prev_tick = this->_tick;
  this->_tick = (cur_ms - this->_start_ms) / this->_tick_ms;
  if ( this->_tick == this->_prev_tick ) {
    return false;
  }
  return true;
} // Effect::tick()

//============================================================================
// class EffectOnly
//----------------------------------------------------------------------------
EffectOnly::EffectOnly(NixieElement *el,
                       unsigned long el_n): Effect::Effect(EFFECT_ONLY,
                                                           el, el_n) {
}

void EffectOnly::start(unsigned long start_ms, unsigned long tick_ms,
                       int el_i, int bl) {
  Effect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  this->_bl = bl;

  for (int i=0; i < this->_el_n; i++) {
    if (i == this->_el_i) {
      this->_el[this->_el_i].set_brightness(this->_bl);
    } else {
      this->_el[this->_el_i].set_brightness(0);
    }
  } // EffectOnly::start()
}

void EffectOnly::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  // do nothing
  return;
} // EffectOnly::loop()
//============================================================================
// class EffectFadeIn
//----------------------------------------------------------------------------
EffectFadeIn::EffectFadeIn(NixieElement *el,
                           unsigned long el_n): Effect::Effect(EFFECT_FADEIN,
                                                               el, el_n) {
}

void EffectFadeIn::start(unsigned long start_ms, unsigned long tick_ms,
                         int el_i) {
  Effect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  //this->_el[this->_el_i].set_brightness(0);
} // EffectFadeIn::start()

void EffectFadeIn::loop(unsigned long cur_ms) {
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
} // EffectFadeIn::loop()
//============================================================================
// class EffectFadeOut
//----------------------------------------------------------------------------
EffectFadeOut::EffectFadeOut(NixieElement *el,
                             unsigned long el_n): Effect::Effect(EFFECT_FADEOUT,
                                                                 el, el_n) {
}

void EffectFadeOut::start(unsigned long start_ms,
                          unsigned long tick_ms,
                          int el_i) {
  Effect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  //this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
}

void EffectFadeOut::loop(unsigned long cur_ms) {
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
} // EffectFadeOut::loop()
//============================================================================
// class EffectXFade
//----------------------------------------------------------------------------
EffectXFade::EffectXFade(NixieElement *el,
                         unsigned long el_n): Effect::Effect(EFFECT_XFADE,
                                                             el, el_n) {
}

void EffectXFade::start(unsigned long start_ms,
                        unsigned long tick_ms,
                        int el_i_in, int el_i_out) {
  Effect::start(start_ms, tick_ms);

  // Serial.println("EffectXFade::start> ");
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
} // EffectXFade::start()

void EffectXFade::loop(unsigned long cur_ms) {
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
} // EffectXFade::loop()
//============================================================================
// class EffectShuffle
//----------------------------------------------------------------------------
EffectShuffle::EffectShuffle(NixieElement *el,
                             unsigned long el_n): Effect(EFFECT_SHUFFLE,
                                                         el, el_n) {
}

void EffectShuffle::start(unsigned long start_ms,
                          unsigned long tick_ms,
                          int n, int el_i) {
  Effect::start(start_ms, tick_ms);

  this->_n = n;
  this->_el_i = el_i;

  for (int e=0; e < this->_el_n; e++) {
    this->_el[e].set_brightness(0);
  }
} // EffectShuffle::start()

void EffectShuffle::loop(unsigned long cur_ms) {
  static int el_random = 0;

  if ( ! this->tick(cur_ms) ) {
    return;
  }

  this->_el[el_random].set_brightness(0);

  if ( this->_tick >= this->_n ) {
    this->end();
    return;
  }
  
  el_random = random(this->_el_n);
  this->_el[el_random].set_brightness(BRIGHTNESS_RESOLUTION);
} // EffectShuffle::loop()

void EffectShuffle::end() {
  Effect::end();

  this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
} // EffectShuffle::end()

//============================================================================
// class EffectBlink
//----------------------------------------------------------------------------
EffectBlink::EffectBlink(NixieElement *el,
                         unsigned long el_n): Effect(EFFECT_BLINK,
                                                     el, el_n) {
} // EffectBlink::EffectBlink()

void EffectBlink::start(unsigned long start_ms,
                        unsigned long tick_ms) {
  Effect::start(start_ms, tick_ms);

  this->_onoff = true;

  for (int i=0; i < this->_el_n; i++) {
    this->_brightness[i] = this->_el[i].get_brightness();
#if 0
    if ( this->_brightness[i] > 0 ) {
      Serial.printf("EffectBlink::start> [%d]->brightness=%d\n",
                    i, this->_brightness[i]);
    }
#endif
  } // for(i)
} // EffectBlink::start()

void EffectBlink::loop(unsigned long cur_ms) {
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
} // EffectBlink::loop()

void EffectBlink::end() {
  Effect::end();

  for (int e=0; e < this->_el_n; e++) {
    this->_el[e].set_brightness(this->_brightness[e]);
  } // for(e)
} // EffectBlink::end()

//============================================================================
// class EffectRandomOnOff
//----------------------------------------------------------------------------
EffectRandomOnOff::EffectRandomOnOff(NixieElement *el,
                                     unsigned long el_n): Effect(EFFECT_RANDOM_ONOFF,
                                                                 el, el_n) {

} // EffectRandomOnOff::EffectRandomOnOff()

void EffectRandomOnOff::start(unsigned long start_ms, unsigned long tick_ms,
                              int el_i) {
  Effect::start(start_ms, tick_ms);

  this->_el_i = el_i;
  this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
} // EffectRandomOnOff::start()

void EffectRandomOnOff::loop(unsigned long cur_ms) {
  if ( ! this->tick(cur_ms) ) {
    return;
  }

  NixieElement *e = &(this->_el[this->_el_i]); // 重要！ポインタ渡し
  if (random(10) > 5) {
    e->set_brightness(BRIGHTNESS_RESOLUTION);
  } else {
    e->set_brightness(0);    
  }
} // EffectRandomOnOff::loop()

void EffectRandomOnOff::end() {
  Effect::end();

  this->_el[this->_el_i].set_brightness(BRIGHTNESS_RESOLUTION);
} // EffectRandomOnOff::end()
