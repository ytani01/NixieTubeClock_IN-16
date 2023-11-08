/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_TEST1_H_
#define _MODE_TEST1_H_

#include "Mode.h"

extern std::string VersionString;

class Mode_Test1: public Mode {
 public:
  static const unsigned long LOOP_DELAY_MS = 10 * 1000; // ms

  int tube = 0;
  int num = 0;

  Mode_Test1();

  void display();

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(const ButtonInfo_t& bi,
                     const std::map<std::string, ButtonInfo_t>& btn_info);
};

#endif // _MODE_TEST1_H_
