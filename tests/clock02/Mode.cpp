/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "Mode.h"

/** constructor
 *
 */
Mode::Mode(String name, CommonData_t *common_data) {
  this->name = name;
  this->common_data = common_data;
  this->_nta = this->common_data->nta;
} // Mode::Mode()

/**
 *
 */
String Mode::get_name() {
    return this->name;
} // Mode::get_name()
  
/**
 * 最初の初期化
 */
void Mode::setup() {
  log_d("%s", this->name.c_str());
} // Mode::setup()

/**
 * モード切替時に毎回実行
 */
bool Mode::enter(Mode_t prev_mode) {
  log_i("%s: prev_mode=%s", this->name.c_str(), MODE_T_STR[prev_mode]);
  this->prev_mode = prev_mode;
  return true;
} // Mode::resume()

/**
 * モード切替時に毎回実行
 */
bool Mode::exit() {
  log_i("%s", this->name.c_str());
  return true;
} // Mode::resume()

/**
 * 
 */
void Mode::loop(unsigned long cur_ms) {
  return;
} // Mode::resume()

/**
 *
 */
Mode_t Mode::btnCb_Mode(ButtonInfo_t *bi) {
  log_i("%s", this->name.c_str());
  return MODE_N;
} // Mode::btnCb_Mode()

/**
 *
 */
Mode_t Mode::btnCb_Up(ButtonInfo_t *bi) {
  log_i("%s", this->name.c_str());
  return MODE_N;
} // Mode::btnCb_Up()

/**
 *
 */
Mode_t Mode::btnCb_Down(ButtonInfo_t *bi) {
  log_i("%s", this->name.c_str());
  return MODE_N;
} // Mode::btnCb_Down()

/**
 *
 */
void Mode::display(Display_t *disp) {
  disp->clearDisplay();
  disp->setCursor(0, 0);
  disp->setTextSize(1);
  disp->setTextColor(BLACK, WHITE);
  disp->printf("%s", __FILE__);
  // disp->display();
} // Mode::display()
