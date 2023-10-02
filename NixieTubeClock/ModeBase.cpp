/*
 * Copyright (c) 2023 Yoichi Tanibayashi
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

  String msg = "";
  msg += "_name=" + this->_name;
  msg += ", ";
  msg += "_tick_ms=" + String(this->_tick_ms);
  msg += ".";
  log_i("%s", msg.c_str());
} // constructor

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
  
  log_i("init_val=[%d %d  %d %d  %d %d]",
        init_val[0], init_val[1],
        init_val[2], init_val[3],
        init_val[4], init_val[5]);

  for (int i = 0; i < NIXIE_NUM_N; i++) {
    this->_num[i] = init_val[i];
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( this->_num[i] == e ) {
        NxNumEl(i, e).set_brightness(Nx->brightness);
      } else {
        NxNumEl(i, e).set_brightness(0);
      }
    } // for(e)
  } // for(i)

  this->stat = STAT_NONE;
  log_i("stat=0x%X", (int)this->stat);
} // ModeBase::init()

/**
 *
 */
stat_t ModeBase::loop(unsigned long cur_ms, DateTime& now) {
  if ( ! this->tick(cur_ms) ) {
    return STAT_SKIP;
  }
  return this->stat;
} // ModeBase::loop()

void ModeBase::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
  log_i("");
} // ModeBase::btn_intr_hdr()

void ModeBase::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  log_i("");
} // ModeBase::btn_loop_hdr()
