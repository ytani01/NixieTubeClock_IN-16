/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NixieTubeArray.h"

//============================================================================
// class NixieTubeArray
//----------------------------------------------------------------------------
/**
 *
 */
NixieTubeArray::NixieTubeArray(uint8_t clk, uint8_t stobe, uint8_t data,
                               uint8_t blank,
                               uint8_t num_pin[NIXIE_NUM_N][NIXIE_NUM_DIGIT_N],
                               uint8_t colon_pin[NIXIE_COLON_N][NIXIE_COLON_DOT_N]) {
  uint8_t pout[] = {clk, stobe, data, blank};

  for (int p=0; p < sizeof(pout) / sizeof(uint8_t); p++) {
    pinMode(pout[p], OUTPUT);
    digitalWrite(pout[p], LOW);
  }
  this->_pin_clk   = clk;
  this->_pin_stobe = stobe;
  this->_pin_data  = data;
  this->_pin_blank = blank;

  for (int n=0; n < NIXIE_NUM_N; n++) {
    this->num[n].setup(NIXIE_NUM_DIGIT_N, num_pin[n]);
    this->prev_num_int[n] = 255;
  } // for(n)

  for (int c=0; c < NIXIE_COLON_N; c++) {
    this->colon[c].setup(NIXIE_COLON_DOT_N, colon_pin[c]);
    this->prev_col_int[c] = 255;
    for (int d=0; d < NIXIE_COLON_DOT_N; d++) {
      pinMode(colon_pin[c][d], OUTPUT);
      digitalWrite(colon_pin[c][d], LOW);
    }
  } // for(c)

  this->_cf_bri = new ConfFile_Brightness();
} // NixieTubeArray::NixieArray()

/**
 *
 */
void NixieTubeArray::loop(unsigned long cur_ms) {
  for (int ti=0; ti < NIXIE_NUM_N; ti++) {
    this->num[ti].loop(cur_ms);
  } // for(ti)
  for (int ti=0; ti < NIXIE_COLON_N; ti++) {
    this->colon[ti].loop(cur_ms);
  } // for(ti)
} // NixieTubeArray::loop()

/**
 *
 */
brightness_t NixieTubeArray::brightness() {
  return this->_cf_bri->brightness;
} // NixieTubeArray::brightness()

/**
 *
 */
brightness_t NixieTubeArray::set_brightness(brightness_t bri) {
  if ( bri <= 0 ) {
    //
    // load config file
    //
    if ( this->_cf_bri->load() <= 0 ) {
      this->_cf_bri->brightness = BRIGHTNESS_RESOLUTION;
      this->_cf_bri->save();
    }
    if ( this->_cf_bri->brightness <= 0 ) {
      this->_cf_bri->brightness = BRIGHTNESS_RESOLUTION;
      this->_cf_bri->save();
    }
  } else {
    //
    // save config file
    //
    this->_cf_bri->brightness = bri;
    this->_cf_bri->save();
  }

  for (int i=0; i < NIXIE_NUM_N; i++) {
    this->num[i].set_brightness(this->_cf_bri->brightness);
  }
  for (int i=0; i < NIXIE_COLON_N; i++) {
    this->colon[i].set_brightness(this->_cf_bri->brightness);
  }
  
  return this->_cf_bri->brightness;
} // NixieTubeArray::set_brightness()

/**
 *
 */
void NixieTubeArray::end_all_effect() {
  for (int t=0; t < NIXIE_NUM_N; t++) {
    this->num[t].end_effect();
  } // for(t)
  for (int c=0; c < NIXIE_COLON_N; c++) {
    this->colon[c].end_effect();
  } // for(c)
} // NixieTubeArray::end_all_effect()

/**
 *
 */
void NixieTubeArray::set_num(uint8_t (&num)[NIXIE_NUM_N],
                             unsigned long xfade_ms) {
  unsigned long cur_ms = millis();

  for (int t=0; t < NIXIE_NUM_N; t++) {
    if ( num[t] == this->prev_num_int[t] ) {
      continue;
    }
    if ( xfade_ms > 0 ) {
      this->num[t].xfade_start(cur_ms, xfade_ms, num[t], this->prev_num_int[t]);
    } else {
      this->num[t].one(num[t]);
    }
    this->prev_num_int[t] = num[t];
  } // for(t)
} // NixieTubeArray::set_num()

/**
 *
 */
void NixieTubeArray::set_col(uint8_t (&col)[NIXIE_COLON_N],
                             unsigned long xfade_ms) {
  unsigned long cur_ms = millis();

  for (int t=0; t < NIXIE_COLON_N; t++) {
    if ( col[t] == this->prev_col_int[t] ) {
      continue;
    }
    if ( xfade_ms > 0 ) {
      //
      // XXX ここで xfadeすると、なぜかパニックで落ちる
      //
      this->colon[t].xfade_start(cur_ms, xfade_ms, col[t], this->prev_col_int[t]);
      //this->colon[t].one(col[t]);
    } else {
      this->colon[t].one(col[t]);
    }
    this->prev_col_int[t] = col[t];
  } // for(t)
} // NixieTubeArray::set_col()

/**
 *
 */
void NixieTubeArray::set_string(std::string str, unsigned long xfade_ms) {
  static std::string prev_str = "";
  
  if ( str != prev_str ) {
    log_v("str = \"%s\"", str.c_str());
    prev_str = str;
  }
  
  str += "AAAAAAAA";
  
  uint8_t num[NIXIE_NUM_N];
  uint8_t col[NIXIE_COLON_N];

  num[0] = str[0] - '0';
  num[1] = str[1] - '0';
  col[0] = str[2] == ' ' ? 10 : 0;
  num[2] = str[3] - '0';
  num[3] = str[4] - '0';
  col[1] = str[5] == ' ' ? 10 : 0;
  num[4] = str[6] - '0';
  num[5] = str[7] - '0';

  //log_d("col[] = {%d, %d}", col[0], col[1]);
  
  this->set_num(num, xfade_ms);
  this->set_col(col, xfade_ms);

  //this->display(millis());
} // NixieTubeArray::set_string()

/** private
 *
 */
void NixieTubeArray::set_onoff(unsigned long cur_ms) {
  uint8_t timing = cur_ms % BRIGHTNESS_RESOLUTION;

  // 数字部
  for (int t=0; t < NIXIE_NUM_N; t++) {
    for (int e=0; e < this->num[t].element_n; e++) {
      if (this->num[t].element[e].brightness() > timing) {
        this->num[t].element[e].on();
      } else {
        this->num[t].element[e].off();
      }
    } // for(e)
  } // for(t)

  // colon部
  for (int t=0; t < NIXIE_COLON_N; t++) {
    for (int e=0; e < this->colon[t].element_n; e++) {
      this->colon[t].element[e].off();
      if (this->colon[t].element[e].brightness() > timing) {
        this->colon[t].element[e].on();
      }
    } // for(e)
  } // for(t)
} // NixieTubeArray::set_onoff()

/**
 *
 */
static unsigned long disp_count=0;
void IRAM_ATTR NixieTubeArray::display(unsigned long cur_ms) {
  uint8_t pin_n = NIXIE_NUM_N * NIXIE_NUM_DIGIT_N;
  uint8_t val[pin_n];

  disp_count = (disp_count + 1) % BRIGHTNESS_RESOLUTION;
  
  //---------------------------------------------------------------------
  this->loop(cur_ms); // ニキシー管 全て
  //---------------------------------------------------------------------
  this->set_onoff(disp_count); // 全エレメントの表示状態更新
  //this->set_onoff(cur_ms);
  //this->set_onoff(micros());
  //---------------------------------------------------------------------
  // 数字部の表示処理
  for (int p=0; p < pin_n; p++) {
    val[p] = HIGH;
  }

  for (int t=0; t < NIXIE_NUM_N; t++) {
    for (int e=0; e < NIXIE_NUM_DIGIT_N; e++) {
      uint8_t pin = this->num[t].element[e].get_pin();

      if ( this->num[t].element[e].is_on() ) {
        val[pin] = LOW;
      }
    } // for(e)
  } // for(t)

  digitalWrite(_pin_clk, LOW);
  for (int p=(pin_n - 1); p >=0; p--) {
    //delayMicroseconds(NixieTubeArray::DISP_DELAY_US);
    digitalWrite(_pin_data, val[p]);
    //delayMicroseconds(NixieTubeArray::DISP_DELAY_US);
    digitalWrite(_pin_clk, HIGH);
    delayMicroseconds(NixieTubeArray::DISP_DELAY_US);
    digitalWrite(_pin_clk, LOW);
  }
  digitalWrite(_pin_stobe, HIGH);
  delayMicroseconds(NixieTubeArray::DISP_DELAY_US);
  digitalWrite(_pin_stobe, LOW);
  delayMicroseconds(NixieTubeArray::DISP_DELAY_US);
  //--------------------------------------------------------------------
  // コロンの表示処理
  for (int c=0; c < NIXIE_COLON_N; c++) {
    for (int d=0; d < NIXIE_COLON_DOT_N; d++) {
      uint8_t pin = this->colon[c].element[d].get_pin();
      if ( this->colon[c].element[d].is_on() ) {
        digitalWrite(pin, HIGH);  // LOW  --> ON !
      } else {
        digitalWrite(pin, LOW); // HIGH --> OFF !
      }
    } // for(d)
  } // for(c)
} // NixieTubeArray::display()
