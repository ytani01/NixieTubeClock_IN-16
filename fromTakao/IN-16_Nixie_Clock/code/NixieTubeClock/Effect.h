/*
 * (c) 2020 Yoichi Tanibayashi
 *
 *----------------------------------------------------------------------------
 * [ Data structure ( not a class tree ) ]
 *
 *  NixieArray
 *   |
 *   +- NixieTube num[NIXIE_NUM_N]
 *   |   |
 *   |   +- NixieElement element[NIXIE_NUM_DIGIT_N]
 *   |   |
 *   |   +- Effect
 *   |
 *   +- NixieTube colon[NIXIE_COLON_N]
 *       |
 *       +- NixieElement element[NIXIE_COLON_DOT_N]
 *       |
 *       +- Effect
 *----------------------------------------------------------------------------
 */
#ifndef EFFECT_H
#define EFFECT_H
#include <Arduino.h>
// #include <RTClib.h>
#include "PCF8563.h"
#include <WiFi.h>
#include "Nixie.h"
#include "NixieElement.h"

typedef unsigned long effect_id_t;
#define EFFECT_NONE          0
#define EFFECT_ONLY          1
#define EFFECT_FADEIN        2
#define EFFECT_FADEOUT       3
#define EFFECT_XFADE         4
#define EFFECT_FOGIN         5
#define EFFECT_FOGOUT        6
#define EFFECT_SHUFFLE       7
#define EFFECT_BLINK         8
#define EFFECT_RANDOM_ONOFF  9

class Effect {
 public:
  Effect(effect_id_t eid, NixieElement *element, unsigned long el_n);
  virtual ~Effect();

  virtual void  start(unsigned long start_ms, unsigned long tick_ms);
  virtual void  start(unsigned long start_ms, unsigned long tick_ms,
                      int element);
  virtual void  start(unsigned long start_ms, unsigned long tick_ms,
                      int el1, int el2);
  virtual void  loop(unsigned long cur_ms);
  virtual void  end();

  effect_id_t get_id();
  boolean     is_active();
  boolean     tick(unsigned long cur_ms);
    
 protected:
  NixieElement *_el;   // array
  unsigned long _el_n      = 0;           // number of elements (10 or 1)
  boolean       _active    = false;
  effect_id_t   _id        = EFFECT_NONE;
  unsigned long _start_ms  = 0;
  unsigned long _tick_ms   = 1000;
  unsigned long _tick      = 0;
  unsigned long _prev_tick = 0;

}; // class Effect
//============================================================================
class EffectOnly : public Effect {
 public:
  EffectOnly(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms, int el_i, int bl);
  void loop(unsigned long cur_ms);

 private:
  int _el_i;
  int _bl;
}; // class EffectOnly
//============================================================================
class EffectFadeIn : public Effect {
 public:
  EffectFadeIn(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms, int element);
  void loop(unsigned long cur_ms);

 private:
  int _el_i;

}; // class EffectFadeIn
//============================================================================
class EffectFadeOut : public Effect {
 public:
  EffectFadeOut(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms, int element);
  void loop(unsigned long cur_ms);

 private:
  int _el_i;

}; // class EffectFadeOut
//============================================================================
class EffectXFade : public Effect {
 public:
  EffectXFade(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms,
             int el_in, int el_out);
  void loop(unsigned long cur_ms);

 private:
  int _el_i_in;
  int _el_i_out;

}; // class EffectXFade

//============================================================================
class EffectShuffle : public Effect {
 public:
  EffectShuffle(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms,
             int n, int el_i);
  void loop(unsigned long cur_ms);
  void end();

 private:
  int   _el_i;
  int   _n;
}; // class EffectShuffle

//============================================================================
class EffectBlink : public Effect {
 public:
  EffectBlink(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms);
  void loop(unsigned long cur_ms);
  void end();

 private:
  int      _el_i;
  uint8_t  _brightness[NIXIE_ELEMENT_N_MAX];
  boolean  _onoff;
}; // class EffectBlink

//============================================================================
class EffectRandomOnOff : public Effect {
 public:
  EffectRandomOnOff(NixieElement *el, unsigned long el_n);
  void start(unsigned long start_ms, unsigned long tick_ms, int el_i);
  void loop(unsigned long cur_ms);
  void end();

 private:
  int _el_i;
}; // class EffectRandomOnOff
#endif // EFFECT_H
