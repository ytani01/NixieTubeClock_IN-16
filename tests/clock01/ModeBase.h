/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _MODE_BASE_H_
#define _MODE_BASE_H_

#include <esp32-hal-log.h>
#include "common.h"
#include "Button.h"
#include "Display.h"

#define _cur_mode this->common_data->cur_mode

/**
 *
 */
class ModeBase {
public:
  ModeBase(String name, CommonData_t *common_data);

  String get_name();

  virtual void setup();
  virtual bool enter(Mode_t prev_mode);
  virtual bool exit();
  virtual void loop(unsigned long cur_ms);

  virtual Mode_t reBtn_cb(ButtonInfo_t *bi);
  virtual Mode_t obBtn_cb(ButtonInfo_t *bi);
  virtual void display(Display_t *disp);

protected:
  String name;
  CommonData_t *common_data;
  Mode_t prev_mode;
}; // class ModeBase

#endif // _MODE_BASE_H_
