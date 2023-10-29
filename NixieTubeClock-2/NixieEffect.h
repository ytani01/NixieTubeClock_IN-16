/*
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ has-a tree ]
 *
 *  NixieTubeArray
 *   |
 *   +- NixieTube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- NixieElement element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- NixieEffect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- NixieEffect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_EFFECT_H
#define NIXIE_EFFECT_H

#include "Nixie.h"
#include "NixieElement.h"

typedef enum {
  NIXIE_EFFECT_NONE,
  NIXIE_EFFECT_ONLY,
  NIXIE_EFFECT_FADEIN,
  NIXIE_EFFECT_FADEOUT,
  NIXIE_EFFECT_XFADE,
  NIXIE_EFFECT_FOGIN,
  NIXIE_EFFECT_FOGOUT,
  NIXIE_EFFECT_SHUFFLE,
  NIXIE_EFFECT_BLINK,
  NIXIE_EFFECT_RANDOM_ONOFF,
  NIXIE_EFFECT_SIZE
} effect_id_t;

/** ==========================================================================
 *
 */
class NixieEffect {
 public:
  NixieEffect(effect_id_t eid, NixieElement *element, unsigned long el_n);
  virtual ~NixieEffect();

  virtual void  start(unsigned long tick_ms, unsigned long start_ms=0);
  virtual void  start(int el_i,
                      unsigned long tick_ms, unsigned long start_ms=0);
  virtual void  start(int el_i1, int el_i2,
                      unsigned long tick_ms, unsigned long start_ms=0);
  virtual void  loop(unsigned long cur_ms=0);
  virtual void  end();

  effect_id_t get_id();
  boolean     is_active();
  boolean     tick(unsigned long cur_ms=0);
    
 protected:
  NixieElement *_el;   // array
  unsigned long _el_n      = 0;           // number of elements (10 or 1)
  boolean       _active    = false;
  effect_id_t   _id        = NIXIE_EFFECT_NONE;
  unsigned long _start_ms  = 0;
  unsigned long _tick_ms   = 1000;
  unsigned long _tick      = 0;
  unsigned long _prev_tick = 0;
  int           _el_random = 0; // for shuffle
}; // class NixieEffect

/** ==========================================================================
 *
 */
class NixieEffectOnly : public NixieEffect {
 public:
  NixieEffectOnly(NixieElement *el, unsigned long el_n);
  void start(int el_i, int bl,
             unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);

 private:
  int _el_i;
  int _bl;
}; // class NixieEffectOnly

/** ==========================================================================
 *
 */
class NixieEffectFadeIn : public NixieEffect {
 public:
  NixieEffectFadeIn(NixieElement *el, unsigned long el_n);
  void start(int el_i, unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  void end();

 private:
  int _el_i;
}; // class NixieEffectFadeIn

/**
 *
 */
class NixieEffectFadeOut : public NixieEffect {
 public:
  NixieEffectFadeOut(NixieElement *el, unsigned long el_n);
  void start(int el_i, unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  void end();

 private:
  int _el_i;
}; // class NixieEffectFadeOut

/** ==========================================================================
 *
 */
class NixieEffectXFade : public NixieEffect {
 public:
  NixieEffectXFade(NixieElement *el, unsigned long el_n);
  void start(int el_src, int el_dst,
             unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);

 private:
  int _el_i_src, _el_i_dst;
}; // class NixieEffectXFade

//============================================================================
class NixieEffectShuffle : public NixieEffect {
 public:
  NixieEffectShuffle(NixieElement *el, unsigned long el_n);
  void start(int n, int el_i,
             unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  void end();

 private:
  int   _el_i;
  int   _n;
}; // class NixieEffectShuffle

//============================================================================
class NixieEffectBlink : public NixieEffect {
 public:
  NixieEffectBlink(NixieElement *el, unsigned long el_n);
  void start(unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  void end();

 private:
  int      _el_i;
  uint8_t  _brightness[NIXIE_ELEMENT_N_MAX];
  boolean  _is_on;
}; // class NixieEffectBlink

//============================================================================
class NixieEffectRandomOnOff : public NixieEffect {
 public:
  NixieEffectRandomOnOff(NixieElement *el, unsigned long el_n);
  void start(int el_i, unsigned long tick_ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  void end();

 private:
  int _el_i;
}; // class NixieEffectRandomOnOff

#endif // NIXIE_EFFECT_H
