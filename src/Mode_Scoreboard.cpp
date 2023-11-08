/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Scoreboard.h"

/**
 *
 */
Mode_Scoreboard::Mode_Scoreboard(): Mode() {
  /*
    load file
  */
  this->conf = new ConfFile_ModeScoreboard();
  this->conf->load();
} // Mode_Scoreboard::Mode_Scoreboard()

void Mode_Scoreboard::display() {
  char str[9];
  sprintf(str, this->NX_FMT1,
          this->conf->score[2], this->conf->score[1], this->conf->score[0]);

  Nxa->set_string(str);

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setCursor(0, 0);

  Disp->setTextWrap(true);
  Disp->setTextSize(1);
  Disp->printf("\n   ");
  Disp->setTextSize(2);
  Disp->printf("%02d %02d %02d",
               this->conf->score[2],
               this->conf->score[1],
               this->conf->score[0]);
  Disp->display();
} // Mode_Scoreboard::display();

/**
 *
 */
int Mode_Scoreboard::inc_score(int i, int n) {
  this->conf->score[i] += n;

  if ( this->conf->score[i] > SCORE_MAX ) {
    this->conf->score[i] = SCORE_MAX;
  }

  this->conf->save();

  return this->conf->score[i];
} // Mode_Scoreboard::inc_score()

/**
 *
 */
int Mode_Scoreboard::dec_score(int i, int n) {
  this->conf->score[i] -= n;

  if ( this->conf->score[i] < 0 ) {
    this->conf->score[i] = 0;
  }

  this->conf->save();

  return this->conf->score[i];
} // Mode_Scoreboard::dec_score()

/** virtual
 *
 */
void Mode_Scoreboard::enter() {
  log_i("enter mode: %s", this->name.c_str());

  this->display();
} // Mode_Scoreboard::enter()

/** virtual
 *
 */
void Mode_Scoreboard::loop() {
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // Mode_Scoreboard::loop()

/**
 *
 */
void Mode_Scoreboard::cbBtn(const ButtonInfo_t& bi,
                           const std::map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(bi).c_str());

  //
  // if文、switch文より、美しい!?
  //
  std::map<std::string, int>btn_i = {
    {"Btn0", 0},
    {"Btn1", 1},
    {"Btn2", 2},
  };
  int b_i = btn_i[bi.name];

  if ( bi.value == Button::ON ) {
    if ( bi.long_pressed ) {
      if ( bi.push_count == 1 ) {
        if ( bi.repeat_count < 6 ) {
          if ( bi.repeat_count % 2 == 0 ) {
            this->dec_score(b_i);
          }
        } else {
          this->dec_score(b_i);
        }
      } else {
        if ( bi.repeat_count > 2 ) {
          Mode::set("Mode_Clock");
          return;
        }
      }
    }
  } else { // OFF
    if ( ! bi.long_pressed ) {
      this->inc_score(b_i, bi.click_count);
    }
  }

  this->display();
} // Mode_Scoreboard::cbBtn()
