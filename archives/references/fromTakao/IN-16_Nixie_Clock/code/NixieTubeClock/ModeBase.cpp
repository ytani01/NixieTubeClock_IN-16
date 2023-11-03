/*
 * (c) 2020 Yoichi Tanibayashi
 */
#include "ModeBase.h"

/**
 *
 */
ModeBase::ModeBase(NixieArray *nxa, String name, unsigned long tick_ms) {
  this->_nxa = nxa;
  this->_name = name;
  this->_tick_ms = tick_ms;
  this->_prev_tick = 0;

  String msg = "ModeBase::setup(): ";
  msg += "_name=" + this->_name;
  msg += ", ";
  msg += "_tick_ms=" + String(this->_tick_ms);
  msg += ".";
  Serial.println(msg);
}

/**
 *
 */
String ModeBase::name() {
  return this->_name;
} // ModeBase::name()

/**
 * tick: loopで処理する間隔
 */
boolean ModeBase::tick(unsigned long cur_ms) {
  if ( this->_tick_ms == 0 ) {
    return true;
    /*
    Serial.println("!? _tick_ms=" + String(this->_tick_ms));
    return false;
    */
  }
  this->_prev_tick = this->_tick;
  this->_tick = (cur_ms - this->_start_ms) / this->_tick_ms;
  if ( this->_tick == this->_prev_tick ) {
    return false;
  }
  return true;
} // ModeBase::tick()

/**
 *
 */
void ModeBase::init(unsigned long start_ms, DateTime& now,
                    int init_val[NIXIE_NUM_N]) {
  this->_start_ms = start_ms;
  (void)this->tick(start_ms);
  
  Serial.printf("ModeBase::init> init_val=[ ");
  for (int i = 0; i < NIXIE_NUM_N; i++) {
    Serial.printf("%d ", init_val[i]);
    this->_num[i] = init_val[i];
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( this->_num[i] == e ) {
        NxNumEl(i, e).set_brightness(Nx->brightness);
      } else {
        NxNumEl(i, e).set_brightness(0);
      }
    } // for(e)
  } // for(i)
  Serial.println("]");

  this->stat = STAT_NONE;
  Serial.printf("ModeBase::init> stat=0x%X\n", (int)this->stat);
} // ModeBase::init()

/**
 *
 */
stat_t ModeBase::loop(unsigned long cur_ms, DateTime& now) {
  if ( ! this->tick(cur_ms) ) {
    return STAT_SKIP;
  }
  // Serial.println("ModeBase::loop()");
  return this->stat;
} // ModeBase::loop()

void ModeBase::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeBase::btn_intr_hdr()");
} // ModeBase::btn_intr_hdr()

void ModeBase::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeBase::btn_loop_hdr()");
} // ModeBase::btn_loop_hdr()
