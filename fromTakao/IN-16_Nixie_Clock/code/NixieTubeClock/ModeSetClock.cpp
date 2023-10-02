/**
 * (c) Yoichi Tanibayashi
 */
#include "ModeSetClock.h"

/**
 *
 */
ModeSetClock::ModeSetClock(NixieArray *nxa): ModeBase::ModeBase(nxa,
                                                                "SetClock",
                                                                ModeSetClock::TICK_MS) {
} // ModeSetClock::ModeSetClock()

/**
 *
 */
mode_t ModeSetClock::change_mode(mode_t mode=ModeSetClock::MODE_NULL) {
  if ( mode != MODE_NULL ) {
    this->_mode = mode;
  } else {
    // mode < 0 .. cyclic
    switch ( this->_mode ) {
    case MODE_YEAR:
      this->_mode = MODE_MONTH;
      break;
    case MODE_MONTH:
      this->_mode = MODE_DAY;
      break;
    case MODE_DAY:
      this->_mode = MODE_HOUR;
      break;
    case MODE_HOUR:
      this->_mode = MODE_MINUTE;
      break;
    default:
      this->_mode = MODE_NULL;
    break;
    } // switch(mode)
  } // if
  Serial.printf("ModeSetClock::change_mode> mode=0x%02X\n", this->_mode);

  int i_1, i_2;
  switch (this->_mode) {
  case MODE_YEAR:   i_1 = 0; i_2 = 1; break;
  case MODE_MONTH:  i_1 = 2; i_2 = 3; break;
  case MODE_DAY:    i_1 = 4; i_2 = 5; break;
  case MODE_HOUR:   i_1 = 0; i_2 = 1; break;
  case MODE_MINUTE: i_1 = 2; i_2 = 3; break;
  default:          i_1 = 4; i_2 = 5; break;
  } // switch(this->_mode)

  for (int i=0; i < NIXIE_NUM_N; i++) {
    NxNum(i).end_effect();

    if ( this->_mode & MODE_DATE ) {
      this->_num[i] = this->_date_num[i];
    } else {
      this->_num[i] = this->_time_num[i];
    }
    // Serial.printf("ModeSetClock::change_mode> num[%d]=%d\n", i, this->_num[i]);

    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      if ( e == this->_num[i] ) {
        NxNumEl(i, e).set_brightness(Nx->brightness);
      } else {
        NxNumEl(i, e).set_brightness(0);
      }
      // Serial.printf("[%d,%d]=%d\n", i, e, NxNumEl(i,e).get_brightness());
    } // for(e)

    if ( i == i_1 || i == i_2 ) {
      // Serial.printf("[%d].blink_start(%d);\n", i, this->_num[i]);
      NxNum(i).blink_start(millis(), BLINK_TICK_MS);
    } // if
    
  } // for(i)

  return this->_mode;
}// ModeSetClock::change_mode()

/**
 *
 */
void ModeSetClock::init(unsigned long start_ms, DateTime& now,
                        int init_val[NIXIE_NUM_N]) {
  char disp_str[NIXIE_NUM_N + 1];
  
  // initialize _date_num[]
  sprintf(disp_str, "%02d%02d%02d",
          now.year() % 100, now.month(), now.day());
  Serial.printf("ModeSetClock::init> disp_str='%s'\n", disp_str);
  for (int i=0; i < NIXIE_NUM_N; i++) {
    this->_date_num[i] = (int)(disp_str[i] - '0');
  } // for(i)

  // initialize _time_num[]
  sprintf(disp_str, "%02d%02d%02d",
          now.hour(), now.minute(), 0);
  Serial.printf("ModeSetClock::init> disp_str='%s'\n", disp_str);
  for (int i=0; i < NIXIE_NUM_N; i++) {
    this->_time_num[i] = (int)(disp_str[i] - '0');
  } // for(i)

  ModeBase::init(start_ms, now, this->_date_num);
  for (int i=0; i < NIXIE_COLON_N; i++) {
    NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(0);
  } // for(i)
  
  Nx->brightness = BRIGHTNESS_RESOLUTION;

  (void)change_mode(ModeSetClock::MODE_YEAR);

  this->stat = STAT_DONE;
} // ModeSetClock::init()

/**
 *
 */
stat_t ModeSetClock::loop(unsigned long cur_ms, DateTime& now) {
  if ( ModeBase::loop(cur_ms, now) == STAT_SKIP ) {
    return STAT_SKIP;
  }
  return this->stat;
} // ModeSetClock::loop()

/**
 *
 */
void ModeSetClock::count_up(int n=1, boolean repeat=false) {
  int num = 0;

  Serial.printf("ModeSetClock::count_up> this->_mode=0x%02X\n", this->_mode);

  switch (this->_mode) {
  case MODE_YEAR:
  case MODE_HOUR:
    num = this->_num[0] * 10 + this->_num[1];
    break;

  case MODE_MONTH:
  case MODE_MINUTE:
    num = this->_num[2] * 10 + this->_num[3];
    break;

  case MODE_DAY:
    num = this->_num[4] * 10 + this->_num[5];
    break;
  } // switch(mode)

  Serial.printf("ModeSetClock::count_up> num=%d\n", num);
  num += n;
  Serial.printf("ModeSetClock::count_up> num=%d\n", num);

  switch (this->_mode) {
  case MODE_YEAR:
    if ( num > 30 ) {
      num = 21;
    }
    this->_date_num[0] = num / 10 % 10;
    this->_date_num[1] = num % 10;
    break;
    
  case MODE_MONTH:
    if ( num > 12 ) {
      num = 1;
    }
    this->_date_num[2] = num / 10 % 10;
    this->_date_num[3] = num % 10;
    break;
    
  case MODE_DAY:
    if ( num > 31 ) {
      num = 1;
    }
    this->_date_num[4] = num / 10 % 10;
    this->_date_num[5] = num % 10;
    break;
    
  case MODE_HOUR:
    if ( num > 23 ) {
      num = 0;
    }
    this->_time_num[0] = num / 10 % 10;
    this->_time_num[1] = num % 10;
    break;

  case MODE_MINUTE:
    if ( num > 59 ) {
      num = 0;
    }
    this->_time_num[2] = num / 10 % 10;
    this->_time_num[3] = num % 10;
    break;

  } // switch(mode)

  change_mode(this->_mode);

} // ModeSetClock::count_up()

/**
 *
 */
void ModeSetClock::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
} // ModeSetClock::btn_intr_hdr()


void ModeSetClock::attachI2C(TwoWire *wire){
  _wire = wire;
}
/**
 *
 */
void ModeSetClock::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  mode_t mode;
  
  if ( btn->get_name() == "BTN0" ) {
    if ( btn->is_long_pressed() && ! btn->is_repeated() ) {
      this->stat = ModeBase::STAT_BACK_MODE;
      return;
    }
  }

  int n = btn->get_click_count();
  if ( btn->is_repeated() ) {
    n = 1;
  }
  if ( n == 0 ) {
    return;
  }

  if ( btn->get_name() == "BTN1" ) {
    for (int i=0; i < n; i++) {
      mode = this->change_mode();

      if ( mode == MODE_NULL ) {
        int year = this->_date_num[0] * 10 + this->_date_num[1] + 2000;
        int month = this->_date_num[2] * 10 + this->_date_num[3];
        int day = this->_date_num[4] * 10 + this->_date_num[5];
        int hour = this->_time_num[0] * 10 + this->_time_num[1];
        int minute = this->_time_num[2] * 10 + this->_time_num[3];

        DateTime now = DateTime(year, month, day, hour, minute, 0);
        // RTC_DS3231 rtc;
        PCF8563 rtc;

        rtc.begin(_wire);
        rtc.adjust(now);
                                
        this->stat = STAT_BACK_MODE;
      }
    } // for(i)
    return;
  }

  if (btn->get_name() == "BTN2" ) {
    for (int i=0; i < n; i++) {
      count_up();
    }
  }
} // ModeSetClock::btn_loop_hdr()
