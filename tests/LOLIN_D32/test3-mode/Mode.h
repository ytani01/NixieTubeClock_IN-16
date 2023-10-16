/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_H_
#define _MODE_H_

#include <Arduino.h>
#include <esp32-hal-log.h>
#include <vector>

/**
 *
 */
class Mode {
public:
  static std::vector<Mode *> mode;
  static Mode *prev_mode;

  String name;

  Mode(String name);

  virtual void setup();
  virtual bool enter();
  virtual bool exit();
  virtual void loop();

}; // class Mode

#endif // _MODE_H_
