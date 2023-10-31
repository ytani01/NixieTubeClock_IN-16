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

/** ==========================================================================
 *
 */
class NixieEffect {
 public:
  NixieEffect(NixieElement *element, unsigned long el_n);
  virtual ~NixieEffect();

  virtual void start(unsigned long ms, unsigned long start_ms=0);
  void loop(unsigned long cur_ms=0);
  virtual void do_loop();
  virtual void end();

  boolean     is_active();
  boolean     tick(unsigned long cur_ms=0);
    
 protected:
  NixieElement *_el;   // array
  unsigned long _el_n      = 0;           // number of elements (10 or 1)
  boolean       _active    = false;
  unsigned long _start_ms  = 0;
  unsigned long _ms        = 1000;
  unsigned long _tick      = 0;
  unsigned long _prev_tick = 0;
}; // class NixieEffect

/** ==========================================================================
 *
 */
class NixieEffectFadeIn : public NixieEffect {
 public:
  NixieEffectFadeIn(int el_i, NixieElement *el, unsigned long el_n);
  virtual void do_loop();
  virtual void end();

 private:
  NixieElement *_el1;
}; // class NixieEffectFadeIn

/** ==========================================================================
 *
 */
class NixieEffectFadeOut : public NixieEffect {
 public:
  NixieEffectFadeOut(int el_i, NixieElement *el, unsigned long el_n);
  virtual void do_loop();
  virtual void end();

 private:
  NixieElement *_el1;
}; // class NixieEffectFadeOut

/** ==========================================================================
 *
 */
class NixieEffectFog : public NixieEffect {
 public:
  NixieEffectFog(int el_i, NixieElement *el, unsigned long el_n);
  virtual void do_loop();
  virtual void end();

 private:
  int _el_i;
  bool _fog_up;
}; // class NixieEffectFog

//============================================================================
class NixieEffectShuffle : public NixieEffect {
 public:
  NixieEffectShuffle(int el_i, int n, NixieElement *el, unsigned long el_n);
  virtual void start(unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
  int _el_i;
  int _n;
  int _el_random;
}; // class NixieEffectShuffle

//============================================================================
class NixieEffectBlink : public NixieEffect {
 public:
  NixieEffectBlink(NixieElement *el, unsigned long el_n);
  virtual void start(unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
  uint8_t  _brightness[NIXIE_ELEMENT_N_MAX];
  boolean  _is_on;
}; // class NixieEffectBlink

#endif // NIXIE_EFFECT_H
