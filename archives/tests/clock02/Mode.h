/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_H_
#define _MODE_H_

#include <esp32-hal-log.h>
#include "common.h"
#include "Button.h"
#include "Display.h"

#define _cur_mode this->common_data->cur_mode

/**
 *
 */
class Mode {
public:
  Mode(String name, CommonData_t *common_data);

  String get_name();

  virtual void setup();
  virtual bool enter(Mode_t prev_mode);
  virtual bool exit();
  virtual void loop(unsigned long cur_ms);

  virtual Mode_t btnCb_Mode(ButtonInfo_t *bi);
  virtual Mode_t btnCb_Up(ButtonInfo_t *bi);
  virtual Mode_t btnCb_Down(ButtonInfo_t *bi);

  virtual void display(Display_t *disp);

protected:
  String name;
  CommonData_t *common_data;
  NixieTubeArray *_nta;
  Mode_t prev_mode;
}; // class Mode

#endif // _MODE_H_
