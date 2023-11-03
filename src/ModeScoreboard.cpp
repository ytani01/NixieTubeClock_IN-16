/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeScoreboard.h"

/**
 *
 */
ModeScoreboard::ModeScoreboard(): Mode() {
  for (int i=0; i < 3; i++) {
    this->score[i] = 0;
  }
} // ModeScoreboard::ModeScoreboard()

/** virtual
 *
 */
void ModeScoreboard::enter() {
  log_i("enter mode: %s", this->name.c_str());

  char str[9];
  sprintf(str, this->NX_FMT1,
          this->score[2], this->score[1], this->score[0]);

  Nxa->set_string(str);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->setTextSize(2);
  Disp->printf("Score\n  Board");
  Disp->display();

  delay(10);
} // ModeScoreboard::enter()

/** virtual
 *
 */
void ModeScoreboard::loop() {
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // ModeScoreboard::loop()

/**
 *
 */
int ModeScoreboard::inc_score(int i, int n) {
  this->score[i] += n;
  if ( this->score[i] > SCORE_MAX ) {
    this->score[i] = SCORE_MAX;
  }
  return this->score[i];
}

/**
 *
 */
int ModeScoreboard::dec_score(int i, int n) {
  this->score[i] -= n;
  if ( this->score[i] < 0 ) {
    this->score[i] = 0;
  }
  return this->score[i];
}

/**
 *
 */
void ModeScoreboard::cbBtn(ButtonInfo_t *bi,
                           std::map<std::string, ButtonInfo_t>& btn_info) {
  log_i("%s", Button::info2String(bi).c_str());

  //
  // if文、switch文より、美しい!?
  //
  std::map<std::string, int>btn_i = {
    {"Btn0", 0},
    {"Btn1", 1},
    {"Btn2", 2},
  };
  int b_i = btn_i[bi->name];

  if ( bi->value == Button::ON ) {
    if ( bi->long_pressed ) {
      if ( bi->push_count == 1 ) {
        if ( bi->repeat_count < 6 ) {
          if ( bi->repeat_count % 2 == 0 ) {
            this->dec_score(b_i);
          }
        } else {
          this->dec_score(b_i);
        }
      } else {
        if ( bi->repeat_count > 3 ) {
          Mode::set("ModeClock");
          return;
        }
      }
    }
  } else { // OFF
    if ( ! bi->long_pressed ) {
      this->inc_score(b_i, bi->click_count);
    }
  }

  char str[9];
  sprintf(str, this->NX_FMT1,
          this->score[2], this->score[1], this->score[0]);

  Nxa->set_string(str);
} // ModeScoreboard::cbBtn()
