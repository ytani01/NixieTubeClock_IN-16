/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode.h"

std::map<std::string, Mode *> Mode::Ent;
Mode *Mode::Cur = NULL;
Mode *Mode::Prev = NULL;

/** constructor
 *
 */
Mode::Mode() {
  log_v("");
} // Mode::Mode()

/** virtual
 *
 * @brief 最初の初期化
 */
void Mode::setup() {
  log_d("%s", this->name.c_str());
} // Mode::setup()

/** virtual
 *
 * @brief モード切替時に毎回実行
 */
void Mode::enter() {
  log_d("enter mode: %s", this->name.c_str());
} // Mode::enter()

/** virtual
 *
 * @brief モード切替時に毎回実行
 */
void Mode::exit() {
  log_d("exit mode %s", this->name.c_str());
  Nxa->end_all_effect();
} // Mode::exit()

/** virtual
 * 
 */
void Mode::loop() {
  log_d("cur_ms=%u", millis());

  delayOrChangeMode(2000);

  return;
} // Mode::loop()

/** virtual
 *
 */
void Mode::cbBtn(ButtonInfo_t *bi, std::map<std::string, bool>& btn_val) {
  log_d("%s", Button::info2String(bi).c_str());
} // Mode::cbBtn()

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
 * @brief  set Mode
 */
void Mode::set(String name) {
  if ( Mode::Ent.size() == 0 ) {
    log_e("Mode::Ent is empty");
    return;
  }

  /*
    set Mode::Prev
  */
  Mode::Prev = Mode::Cur;
  std::string prev_mode_name = "(NULL)";
  if ( Mode::Prev ) {
    prev_mode_name = Mode::Prev->name.c_str(); // String --> std::string
  }
  log_i("%s --> %s ...", prev_mode_name.c_str(), name.c_str());

  /*
    set Mode::Cur
  */
  std::string cur_mode_name = name.c_str(); // String -> std::string
  try {
    Mode::Cur = Mode::Ent.at(cur_mode_name);
  } catch (const std::out_of_range e) {
    log_e("Mode:%s is invalid", cur_mode_name.c_str());
    Mode::Cur = Mode::Prev;
    return;
  }

  Flag_ReqModeChange = true;
  {
    /*
      wait prev-mode loop
    */
    unsigned long wait_count = 0;
    while ( Flag_LoopRunning ) {
      //delay(1);
      wait_count++;
      auto xLastTime = xTaskGetTickCount();
      vTaskDelayUntil(&xLastTime, 1);
    }
    if ( wait_count > 0 ) {
      log_d("wait_count=%u", wait_count);
    }

    /*
      exit prev-mode
    */
    if ( Mode::Prev ) {
      Mode::Prev->exit();
    }

    /*
      enter cur-mode
    */
    Mode::Cur->enter();

    log_d("%s --> %s: done", prev_mode_name.c_str(), Mode::Cur->name.c_str());
  }
  Flag_ReqModeChange = false;
  
} // Mode::set()

/** static
 *
 */
void Mode::disp_spin(unsigned long interval_ms) {
  switch ( millis() / interval_ms % 8 ) {
  case 0: Disp->printf("|"); break;
  case 1: Disp->printf("/"); break;
  case 2: Disp->printf("-"); break;
  case 3: Disp->printf("\\"); break;
  case 4: Disp->printf("|"); break;
  case 5: Disp->printf("/"); break;
  case 6: Disp->printf("-"); break;
  case 7: Disp->printf("\\"); break;
  default: Disp->printf(" "); break;
  } // switch
} // Mode::disp_spin()  
