/*
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ has-a tree ]
 *
 *  Nixie_TubeArray
 *   |
 *   +- Nixie_Tube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- Nixie_Element element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- Nixie_Effect
 *   |
 *   +- Nixie_Tube colon[NIXIE_COLON_N]
 *       |
 *       +- Nixie_Element element[NIXIE_COLON_DOT_N]
 *       |
 *       +- Nixie_Effect
 *----------------------------------------------------------------------------
 */
#ifndef NIXIE_EFFECT_H
#define NIXIE_EFFECT_H

#include "Nixie.h"
#include "Nixie_Element.h"

/** ==========================================================================
 *
 */
class Nixie_Effect {
 public:
  Nixie_Effect(Nixie_Element *element, unsigned long el_n);
  virtual ~Nixie_Effect();

  virtual void start(unsigned long ms, unsigned long start_ms=0);
  virtual void start(int el_i, unsigned long ms, unsigned long start_ms=0);
  virtual void start(int el_i, int n, unsigned long ms, unsigned long start_ms=0);

  void loop(unsigned long cur_ms=0);
  virtual void do_loop();

  virtual void end();

  boolean     is_active();
  boolean     tick(unsigned long cur_ms=0);
    
 protected:
  int _el_i;
  int _n;
  Nixie_Element *_el1;

  Nixie_Element *_el;   // array
  unsigned long _el_n      = 0;           // number of elements (10 or 1)

  boolean       _active    = false;
  unsigned long _start_ms  = 0;
  unsigned long _ms        = 1000;
  unsigned long _tick      = 0;
  unsigned long _prev_tick = 0;
}; // class Nixie_Effect

/** ==========================================================================
 *
 */
class Nixie_EffectFadeIn : public Nixie_Effect {
 public:
  Nixie_EffectFadeIn(Nixie_Element *element, unsigned long el_n);
  virtual void start(int el_i, unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
}; // class Nixie_EffectFadeIn

/** ==========================================================================
 *
 */
class Nixie_EffectFadeOut : public Nixie_Effect {
 public:
  Nixie_EffectFadeOut(Nixie_Element *element, unsigned long el_n);
  virtual void start(int el_i, unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
}; // class Nixie_EffectFadeOut

/** ==========================================================================
 *
 */
class Nixie_EffectFog : public Nixie_Effect {
 public:
  Nixie_EffectFog(Nixie_Element *element, unsigned long el_n);
  virtual void start(int el_i, unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
  bool _fog_up;
}; // class Nixie_EffectFog

//============================================================================
class Nixie_EffectShuffle : public Nixie_Effect {
 public:
  Nixie_EffectShuffle(Nixie_Element *element, unsigned long el_n);
  virtual void start(int el_i, int n,
                     unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
  int _el_random;
}; // class Nixie_EffectShuffle

//============================================================================
class Nixie_EffectBlink : public Nixie_Effect {
 public:
  Nixie_EffectBlink(Nixie_Element *element, unsigned long el_n);
  virtual void start(unsigned long ms, unsigned long start_ms=0);
  virtual void do_loop();
  virtual void end();

 private:
  uint8_t  _brightness[NIXIE_ELEMENT_N_MAX];
  boolean  _is_on;
}; // class Nixie_EffectBlink

#endif // NIXIE_EFFECT_H
