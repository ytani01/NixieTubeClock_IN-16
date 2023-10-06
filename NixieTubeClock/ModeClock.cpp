/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeClock.h"

static const unsigned int CL_FADE_OFF = 0;
static const unsigned int CL_FADE_IN  = 1;
static const unsigned int CL_FADE_OUT = 2;
int colon_fade_mode[NIXIE_COLON_N] = {CL_FADE_OFF, CL_FADE_OFF};

static const unsigned int CL_FADE_TICK = 40;
static unsigned long clFadeTick = CL_FADE_TICK; // ticks

extern boolean wifiActive;

static DateTime prev_dt = DateTime(2000,1,1,0,0,0);

/**
 *
 */
ModeClock::ModeClock(NixieArray *nxa)
  : ModeBase::ModeBase(nxa, "Clock", ModeClock::TICK_MS) {
} // ModeClock::ModeClock()

/**
 *
 */
void ModeClock::init(unsigned long start_ms, DateTime& now,
                     int init_val[NIXIE_NUM_N]) {
  ModeBase::init(start_ms, now, init_val);
  this->mode_start_ms = millis();
  log_i("mode_start_ms=%ld, stat=0x%X",
        this->mode_start_ms, (int)this->stat);

  Nx->brightness = this->brightness;
}

/**
 *
 */
stat_t ModeClock::loop(unsigned long cur_ms, DateTime& now) {
  char disp_str[2 * 3 + 1];
  int  prev_num[NIXIE_NUM_N];
  static unsigned long prev_mode = MODE_NULL;

  if ( ModeBase::loop(cur_ms, now) == STAT_SKIP ) {
    return STAT_SKIP;
  }

  if ( this->stat != STAT_NONE && this->stat != STAT_DONE ) {
    log_i("stat=0x%X", (int)this->stat);
    return this->stat;
  }

  switch ( this->mode ) {
  case ModeClock::MODE_HMS:
    sprintf(disp_str, "%02d%02d%02d", now.hour(), now.minute(), now.second());
    break;

  case ModeClock::MODE_DHM:
    sprintf(disp_str, "%02d%02d%02d", now.day(), now.hour(), now.minute());
    break;

  case ModeClock::MODE_YMD:
    if ( cur_ms - this->mode_start_ms > DISP_DATE_MS ) {
      this->change_mode(MODE_HMS);

      this->stat = STAT_DONE;
      return this->stat;
    }
    sprintf(disp_str, "%02d%02d%02d",
            now.year() % 100, now.month(), now.day());
    break;

  case ModeClock::MODE_MDH:
    if ( cur_ms - this->mode_start_ms > DISP_DATE_MS ) {
      this->change_mode(MODE_HMS);

      this->stat = STAT_DONE;
      return this->stat;
    }
    sprintf(disp_str, "%02d%02d%02d",
            now.month(), now.day(), now.hour());
    break;

  default:
    sprintf(disp_str, "%02d%02d%02d",
            now.hour(), now.minute(), now.second());
    break;
  } // switch (mode)

  clFadeTick = CL_FADE_TICK * ( BRIGHTNESS_RESOLUTION / Nx->brightness );
  log_d("clFadeTick=%d", clFadeTick);

  for (int i=0; i < NIXIE_NUM_N; i++) {
    prev_num[i] = this->_num[i];
    this->_num[i] = int(disp_str[i] - '0');

    if ( this->_num[i] != prev_num[i] ) {
      //NxNum(i).xfade_start(cur_ms, FADE_TICK_MS, this->_num[i], prev_num[i]);
      NxNum(i).xfade_start(cur_ms, clFadeTick, this->_num[i], prev_num[i]);
    }
  } // for(NUM)

  for (int i=0; i < NIXIE_COLON_N; i++) {
    if ( this->mode == ModeClock::MODE_YMD ) {
      NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(0);
      continue;
    }

    if ( wifiActive ) {
      if ( prev_dt.second() != now.second() ) {
        if ( now.second() % 2 == 1 ) {
          NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(Nx->brightness);
          NxCol(i).fadeout_start(cur_ms, clFadeTick, NIXIE_COLON_DOT_DOWN);
          colon_fade_mode[i] = CL_FADE_OUT;
          continue;
        } else {
          NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(0);
          NxCol(i).fadein_start(cur_ms, clFadeTick, NIXIE_COLON_DOT_DOWN);
          colon_fade_mode[i] = CL_FADE_IN;
          continue;
        }
      }
    } else {
      NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(Nx->brightness);
    }
  } // for(COLON)
  prev_dt = DateTime(now);

  this->stat = STAT_DONE;
  return this->stat;
} // ModeClock::loop()

void ModeClock::change_mode(unsigned long mode=ModeClock::MODE_NULL) {
  log_i("mode=%ld", this->mode);

  if ( mode != MODE_NULL ) {
    this->mode = mode;
  } else {
    switch ( this->mode ) {
    case MODE_HMS:
      this->mode = MODE_YMD;
      break;
    case MODE_DHM:
      this->mode = MODE_YMD;
      break;
    case MODE_YMD:
      this->mode = MODE_HMS;
      break;
    default:
      this->mode = MODE_HMS;
      break;
    } // switch(mode)
  }
  this->mode_start_ms = millis();
  log_i("mode=%ld, mode_start_ms=%ld", this->mode, this->mode_start_ms);
}

void ModeClock::btn_intr_hdr(unsigned long cur_ms, Button *btn) {
} // ModeClock::btn_intr_hdr()

void ModeClock::btn_loop_hdr(unsigned long cur_ms, Button *btn) {
  boolean      flag = false;
  unsigned int bl = Nx->brightness;
  
  // BTN0 or BTN1 or BTN2
  if ( btn->get_name() == "BTN0" ) {
    // BTN0
    if ( btn->is_long_pressed() && ! btn->is_repeated() ) {
      this->stat = ModeBase::STAT_NEXT_MODE;
      log_i("stat=0x%X", (int)this->stat);
      return;
    }
    return;
  }

  // BTN1 or BTN2
  if ( btn->get_name() == "BTN1" ) {
    // BTN1
    if ( btn->get_click_count() >= 1 ) {
      this->change_mode();
    }
    return;
  }

  // BTN2
  if ( btn->get_click_count() == 0 ) {
    return;
  }
  
  if ( btn->get_click_count() == 1 ) {
    bl /= 2;
    if (bl < BRIGHTNESS_MIN || bl > BRIGHTNESS_RESOLUTION) {
      bl = BRIGHTNESS_RESOLUTION;
    }
  
    Nx->brightness = bl;
    this->brightness = bl;
    log_i("Nx->brightness=%d\n", Nx->brightness);
    for (int i=0; i < NIXIE_NUM_N; i++) {
      NxNumEl(i, this->_num[i]).set_brightness(bl);
    } // for(NUM)
  
    for (int i=0; i < NIXIE_COLON_N; i++) {
      NxColEl(i, NIXIE_COLON_DOT_DOWN).set_brightness(bl);
    } // for(COLON)

    return;
  } // if

  // BTN2 and click_count > 1
} // ModeClock::btn_loop_hdr()
