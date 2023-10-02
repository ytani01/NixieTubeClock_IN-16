/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "NixieArray.h"

//============================================================================
// class NixieArray
//----------------------------------------------------------------------------
NixieArray::NixieArray(uint8_t clk, uint8_t stobe, uint8_t data, uint8_t blank,
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
  } // for(n)

  for (int c=0; c < NIXIE_COLON_N; c++) {
    this->colon[c].setup(NIXIE_COLON_DOT_N, colon_pin[c]);
    for (int d=0; d < NIXIE_COLON_DOT_N; d++) {
      pinMode(colon_pin[c][d], OUTPUT);
      digitalWrite(colon_pin[c][d], LOW);
    }
  } // for(c)

} // NixieArray::setup()

void NixieArray::loop(unsigned long cur_ms) {
  for (int ti=0; ti < NIXIE_NUM_N; ti++) {
    this->num[ti].loop(cur_ms);
  } // for(ti)
  for (int ti=0; ti < NIXIE_COLON_N; ti++) {
    this->colon[ti].loop(cur_ms);
  } // for(ti)
} // NixieArray::loop()

void NixieArray::end_all_effect() {
  for (int t=0; t < NIXIE_NUM_N; t++) {
    this->num[t].end_effect();
  } // for(t)
  for (int c=0; c < NIXIE_COLON_N; c++) {
    this->colon[c].end_effect();
  } // for(c)
} // NixieArray::end_all_effect()

/**
 *
 */
void NixieArray::set_onoff(unsigned long cur_ms) {
  uint8_t timing = cur_ms % BRIGHTNESS_RESOLUTION;

  // 数字部
  for (int t=0; t < NIXIE_NUM_N; t++) {
    for (int e=0; e < this->num[t].element_n; e++) {
      if (this->num[t].element[e].get_brightness() > timing) {
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
      if (this->colon[t].element[e].get_brightness() > timing) {
        this->colon[t].element[e].on();
      }
    } // for(e)
  } // for(t)
} // NixieArray::set_onoff()

/**
 *
 */
static unsigned long disp_count=0;
void IRAM_ATTR NixieArray::display(unsigned long cur_ms) {
  uint8_t pin_n = NIXIE_NUM_N * NIXIE_NUM_DIGIT_N;
  uint8_t val[pin_n];

  disp_count++;
  disp_count %= BRIGHTNESS_RESOLUTION;
  
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
    //delayMicroseconds(NixieArray::DISP_DELAY_US);
    digitalWrite(_pin_data, val[p]);
    //delayMicroseconds(NixieArray::DISP_DELAY_US);
    digitalWrite(_pin_clk, HIGH);
    delayMicroseconds(NixieArray::DISP_DELAY_US);
    digitalWrite(_pin_clk, LOW);
  }
  digitalWrite(_pin_stobe, HIGH);
  delayMicroseconds(NixieArray::DISP_DELAY_US);
  digitalWrite(_pin_stobe, LOW);
  delayMicroseconds(NixieArray::DISP_DELAY_US);
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
} // NixieArray::display()
