/**
 * (c) 2020 Yoichi Tanibayashi
 */
#include "ModeTest1.h"

ModeTest1::ModeTest1(NixieArray *nxa):  ModeBase::ModeBase(nxa,
                                                           "ModeTest1",
                                                           ModeTest1::TICK_MS) {
}

void ModeTest1::init(unsigned long start_ms, DateTime& now,
                     int init_val[NIXIE_NUM_N]) {
  Serial.println("ModeTest1::init>");

  ModeBase::init(start_ms, now, init_val);

  this->_digit = 0;
  this->_prev_digit = 9;

  for (int i=0; i < NIXIE_NUM_N; i++) {
    for (int d=0; d < NIXIE_NUM_DIGIT_N; d++) {
      if (d == this->_prev_digit) {
        this->_nxa->num[i].element[d].set_brightness(BRIGHTNESS_RESOLUTION);
      } else {
        this->_nxa->num[i].element[d].set_brightness(0);
      }
    } // for(d)
  } // for(i)

  for (int i=0; i < NIXIE_COLON_N; i++) {
    Serial.println("i=" + String(i));
    for (int d=0; d < NIXIE_COLON_DOT_N; d++) {
      Serial.println(" d=" + String(d));
      this->_nxa->colon[i].element[d].set_brightness(BRIGHTNESS_RESOLUTION);
    } // for (d)
    this->_nxa->colon[i].blink_start(start_ms, BLINK_TICK_MS);
  } // for (i)

  Serial.println("ModeTest1::init> done");
}

stat_t ModeTest1::loop(unsigned long cur_ms, DateTime& now) {
  if ( ModeBase::loop(cur_ms, now) == STAT_SKIP ) {
    return STAT_SKIP;
  }
  // Serial.println("ModeTest1::loop> digit=" + String(this->_digit));

  // -------------------------------------------------------------------------
  // num
  int num = 0;
  this->_nxa->num[num].element[this->_prev_digit].set_brightness(0);
  this->_nxa->num[num].element[this->_digit].set_brightness(BRIGHTNESS_RESOLUTION);
  //this->_nxa->num[num].element[this->_digit].set_brightness(1);
  
  num = 1;
  this->_nxa->num[num].element[this->_prev_digit].set_brightness(0);
  this->_nxa->num[num].fadein_start(cur_ms, this->FADE_TICK_MS,
                                    this->_digit);

  num = 2;
  this->_nxa->num[num].element[this->_digit].set_brightness(BRIGHTNESS_RESOLUTION);
  this->_nxa->num[num].fadeout_start(cur_ms, this->FADE_TICK_MS,
                                     this->_prev_digit);
  
  num = 3;
  this->_nxa->num[num].xfade_start(cur_ms, this->FADE_TICK_MS,
                                   this->_digit, this->_prev_digit);

  num = 4;
  this->_nxa->num[num].shuffle_start(cur_ms, this->SHUFFLE_TICK_MS,
                                     this->SHUFFLE_COUNT, this->_digit);

  num = 5;
  this->_nxa->num[num].end_effect();
  this->_nxa->num[num].element[this->_prev_digit].set_brightness(0);
  this->_nxa->num[num].element[this->_digit].set_brightness(BRIGHTNESS_RESOLUTION);
  this->_nxa->num[num].blink_start(cur_ms, BLINK_TICK_MS);
  
  // -------------------------------------------------------------------------
  this->_prev_digit = this->_digit;
  this->_digit = (this->_digit + 1) % 10;

  return STAT_DONE;
} // ModeTest1::loop()

void ModeTest1::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeTest1::btn_intr_hdr(" + btn->get_name() + ")");
} // ModeTest1::btn_intr_hdr()

void ModeTest1::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  Serial.println("ModeTest1::btn_loop_hdr(" + btn->get_name() + ")");
} // ModeTest1::btn_loop_hdr()
