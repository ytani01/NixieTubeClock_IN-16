/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_H_
#define _MODE_H_

#include <Arduino.h>
#include <esp32-hal-log.h>
#include <map>
#include "commonlib.h"
#include "SysClock.h"

/**
 *
 */
class Mode {
public:
  static std::map<std::string, Mode *> Ent;
  static Mode *Cur;
  static Mode *Prev;

  String name = "";

  Mode();

  static void add(String name, Mode *mode);
  static void set(String name);

  virtual void setup();
  virtual bool enter();
  virtual void loop();
  virtual bool exit();

protected:
}; // class Mode

#endif // _MODE_H_
