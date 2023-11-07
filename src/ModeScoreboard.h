/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_SCOREBOARD_H_
#define _MODE_SCOREBOARD_H_

#include "Mode.h"
#include "ConfFile_ModeScoreboard.h"

class ModeScoreboard: public Mode {
 public:
  static const unsigned long LOOP_DELAY_MS = 10000; // ms
  static constexpr char* NX_FMT1 = (char *)"%2d %2d %2d";

  static const int SCORE_MAX = 99;

  ConfFile_ModeScoreboard *conf;

  ModeScoreboard();

  void display();
  int inc_score(int i, int n=1);
  int dec_score(int i, int n=1);

  virtual void enter();
  virtual void loop();

  virtual void cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info);
};

#endif // _MODE_SCOREBOARD_H_
