/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode.h"

std::map<std::string, Mode *> Mode::Ent;
Mode *Mode::Cur = (Mode *)NULL;
Mode *Mode::Prev = (Mode *)NULL;

/** static
 *
 */
void Mode::add(String name, Mode *mode) {
  std::string name_string = name.c_str();

  Mode::Ent.emplace(name_string, mode);

  Mode::Ent.at(name_string)->name = name;

  log_d("size: %d, name: %s, mode: 0x%X",
        Mode::Ent.size(), name_string.c_str(), mode);
} // Mode::add()

/** static
 *
 */
void Mode::set(String name) {
  if ( Mode::Ent.size() == 0 ) {
    log_e("Mode::Ent is empty");
    return;
  }

  Mode::Prev = Mode::Cur;

  String prev_name = "";
  if ( Mode::Prev ) {
    Mode::Prev->exit();

    prev_name = Mode::Prev->name; // String --> std::string
  }

  std::string name_string = name.c_str();
  try {
    Mode::Cur = Mode::Ent.at(name_string);
  }
  catch (const std::out_of_range e) {
    log_e("Mode:%s is not found", name_string.c_str());
    return;
  }

  log_d("%s --> %s", prev_name, Mode::Cur->name.c_str());

  Mode::Cur->enter();
  
} // Mode::set()

/** constructor
 *
 */
Mode::Mode() {
  log_v("");
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
  log_d("enter mode: %s", this->name.c_str());
  return true;
} // Mode::resume()

/**
 * モード切替時に毎回実行
 */
bool Mode::exit() {
  log_d("exit mode %s", this->name.c_str());
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

/**
 *
 */
void Mode::cbBtn(ButtonInfo_t *bi) {
  log_d("%s", Button::info2String(bi).c_str());
} // Mode::cbBtn()
