/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeTest2.h"

/**
 *
 */
ModeTest2::ModeTest2(NixieArray *nxa): ModeBase::ModeBase(nxa,
                                                          "ModeTest2",
                                                          ModeTest2::TICK_MS) {
  this->_cur = 0;
} // ModeTest2::ModeTest2()

/**
 *
 */
void ModeTest2::init(unsigned long start_ms, DateTime& now,
                     int init_val[NIXIE_NUM_N]) {
  Serial.println("ModeTest2::init>");

  ModeBase::init(start_ms, now, init_val);

  this->_cur = 0;

  for (int i=0; i < NIXIE_NUM_N; i++) {
    Serial.println("i=" + String(i));
    this->_digit[i] = i;
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( e == this->_digit[i] ) {
        NxNumEl(i,e).set_brightness(BRIGHTNESS_RESOLUTION);
      } else {
        NxNumEl(i,e).set_brightness(0);
      }
    } // for(e)
  } // for(i)
  this->_nxa->num[this->_cur].end_effect();
  this->_nxa->num[this->_cur].blink_start(start_ms, 200);
} // ModeTest2::init()

/**
 *
 */
stat_t ModeTest2::loop(unsigned long cur_ms, DateTime& now) {
  if ( ModeBase::loop(cur_ms, now) == STAT_SKIP ) {
    return STAT_SKIP;
  }

  String msg = "_digit=[ ";
  for (int i=0; i < NIXIE_NUM_N; i++) {
    boolean e_flag = false;

    if ( NxNum(i).effect_is_active() ) {
      NxNum(i).end_effect();
      e_flag = true;
    }
    NxNumEl(i, this->_digit[i]).set_brightness(0);

    this->_digit[i] = (this->_digit[i] + 10 + 1) % 10;
    NxNumEl(i, this->_digit[i]).set_brightness(BRIGHTNESS_RESOLUTION);
    if ( e_flag ) {
      NxNum(i).blink_start(cur_ms, 200);
    }

    if (i == this->_cur) {
      msg += "<" + String(this->_digit[i]) + "> ";
    } else {
      msg += String(this->_digit[i]) + " ";
    }
  } // for(i)
  msg += "]";
  Serial.println(msg);

  return STAT_DONE;
} // ModeTest2::loop()

/**
 *
 */
void ModeTest2::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeTest2::btn_intr_hdr(" + btn->get_name() + ")");

} // ModeTest2::btn_intr_hdr()

/**
 *
 */
void ModeTest2::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeTest2::btn_loop_hdr(" + btn->get_name() + ")");

  if ( btn->get_name() == "BTN1" ) {
    count_t click_count = btn->get_click_count();
    Serial.println("click_count=" + String(click_count));
    boolean is_repeated = btn->is_repeated();
    Serial.println("is_repeated=" + String(is_repeated));

    if ( click_count > 0 || is_repeated ) {
      for (int i=0; i < NIXIE_NUM_N; i++) {
        NxNum(i).end_effect();
      }
      this->_cur = (this->_cur + 1) % NIXIE_NUM_N;
      NxNum(this->_cur).blink_start(cur_ms, 200);
    }
  }
} // ModeTest2::btn_loop_hdr()
