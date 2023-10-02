/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "ModeCount.h"

extern const char COUNT_FILE[];

/**
 *
 */
ModeCount::ModeCount(NixieArray *nxa): ModeBase::ModeBase(nxa, "ModeCount",
                                                          ModeCount::TICK_MS) {
  for (int i=0; i < COUNT_N; i++) {
    this->data.count[i] = 0;
  }
  this->_cur = 0;
  this->_mode = MODE_NONE;
} // ModeCount::ModeCount()

/**
 *
 */
void ModeCount::init(unsigned long start_ms, DateTime& now,
                     int init_val[NIXIE_NUM_N]) {
  Serial.println("ModeCount::init>");

  this->data.load();

  for (int i=0; i < 3; i++) {
    init_val[i * 2] =     this->data.count[i] / 10;
    init_val[i * 2 + 1] = this->data.count[i] % 10;
  }
  
  ModeBase::init(start_ms, now, init_val);

  this->stat = STAT_DONE;
} // ModeCount::init()

/**
 *
 */
stat_t ModeCount::loop(unsigned long cur_ms, DateTime& now) {
  const char* myname = "ModeCount::loop";

  if ( ModeBase::loop(cur_ms, now) == STAT_SKIP ) {
    return STAT_SKIP;
  }

  if ( this->_mode == MODE_MODIFY ) {
    if ( cur_ms - this->_modify_ms > this->MODIFY_TIMEOUT ) {
      Serial.printf("%s> MODIFY_TIMEOUT\n", myname);

      int n1 = 0, n2 = 1;
      switch ( this->_cur ) {
      case 0: n1 = 0; n2 = 1; break;
      case 1: n1 = 2; n2 = 3; break;
      case 2: n1 = 4; n2 = 5; break;
      } // switch(cur)

      this->_mode = MODE_NONE;

      NxNum(n1).end_effect();
      NxNum(n2).end_effect();
    }
  } // if(MODE_MODIFY)

  return STAT_DONE;
} // ModeCount::loop()

/**
 *
 */
void ModeCount::get_disp_str(char* disp_str) {
  sprintf(disp_str, "%02d%02d%02d",
          this->data.count[0], this->data.count[1], this->data.count[2]);
  Serial.printf("%s\n", disp_str);
} // ModeCount::get_disp_str()

/**
 *
 */
void ModeCount::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
} // ModeCount::btn_intr_hdr()

/**
 *
 */
void ModeCount::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  int n = btn->get_click_count();

  int n1 = 0, n2 = 1;
  switch ( this->_cur ) {
  case 0: n1 = 0; n2 = 1; break;
  case 1: n1 = 2; n2 = 3; break;
  case 2: n1 = 4; n2 = 5; break;
  } // switch
  
  if ( btn->get_name() == "BTN0" ) {
    if ( btn->is_long_pressed() && ! btn->is_repeated() ) {
      if ( this->_mode == MODE_NONE ) {
        this->_mode = MODE_MODIFY;

        this->_modify_ms = cur_ms;

        NxNum(n1).blink_start(millis(), BLINK_TICK_MS);
        NxNum(n2).blink_start(millis(), BLINK_TICK_MS);

      } else {
        this->_mode = MODE_NONE;

        NxNum(n1).end_effect();
        NxNum(n2).end_effect();
      }
      return;
    }

    if ( this->_mode != MODE_MODIFY ) {
      return;
    }

    if ( btn->get_value() == Button::ON ) {
      n = 0;
    }

    if ( n == 0 ) {
      return;
    }

    this->_modify_ms = cur_ms;

    NxNum(n1).end_effect();
    NxNum(n2).end_effect();

    this->_cur = (this->_cur + n) % COUNT_N;
    Serial.printf("ModeCount::btn_loop_hdr> cur=%d\n", this->_cur);

    switch ( this->_cur ) {
    case 0: n1 = 0; n2 = 1; break;
    case 1: n1 = 2; n2 = 3; break;
    case 2: n1 = 4; n2 = 5; break;
    } // switch
    
    NxNum(n1).blink_start(millis(), BLINK_TICK_MS);
    NxNum(n2).blink_start(millis(), BLINK_TICK_MS);

    return;
  }

  if ( this->_mode != MODE_MODIFY ) {
    return;
  }

  if ( btn->is_repeated() ) {
    n = 1;
  }
  
  if ( n <= 0 ) {
    return;
  }

  this->_modify_ms = cur_ms;

  NxNum(n1).end_effect();
  NxNum(n2).end_effect();

  NxNumEl(n1, this->data.count[this->_cur] / 10).set_brightness(0);
  NxNumEl(n2, this->data.count[this->_cur] % 10).set_brightness(0);

  if ( btn->get_name() == "BTN1" ) {
    this->data.count[this->_cur] += n;
    if ( this->data.count[this->_cur] > 99 ) {
      this->data.count[this->_cur] = 99;
    }
    Serial.printf("ModeCount::btn_loop_hdr> score[%d]=%d\n",
                  this->_cur, this->data.count[this->_cur]);


    this->data.save(COUNT_FILE);
  }

  if ( btn->get_name() == "BTN2" ) {
    this->data.count[this->_cur] -= n;
    if ( this->data.count[this->_cur] < 0 ) {
      this->data.count[this->_cur] = 0;
    }
    Serial.printf("ModeCount::btn_loop_hdr> score[%d]=%d\n",
                  this->_cur, this->data.count[this->_cur]);
  }

  NxNumEl(n1, this->data.count[this->_cur] / 10).set_brightness(Nx->brightness);
  NxNumEl(n2, this->data.count[this->_cur] % 10).set_brightness(Nx->brightness);

  NxNum(n1).blink_start(millis(), BLINK_TICK_MS);
  NxNum(n2).blink_start(millis(), BLINK_TICK_MS);
  
} // ModeCount::btn_loop_hdr()
