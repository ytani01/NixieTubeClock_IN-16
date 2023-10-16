/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode.h"

/** constructor
 *
 */
Mode::Mode(String name) {
  this->name = name;
} // Mode::Mode()

/**
 * 最初の初期化
 */
void Mode::setup() {
  log_d("%s", this->name.c_str());
} // Mode::setup()

/**
 * モード切替時に毎回実行
 */
bool Mode::enter() {
  log_d("mode: %s --> %s", Mode::prev_mode->name.c_str(), this->name.c_str());
  return true;
} // Mode::resume()

/**
 * モード切替時に毎回実行
 */
bool Mode::exit() {
  log_d("%s", this->name.c_str());
  return true;
} // Mode::resume()

/**
 * 
 */
void Mode::loop() {
  unsigned long cur_ms = millis();

  log_d("cur_ms=%u", cur_ms);

  delay(1000);

  return;
} // Mode::resume()
