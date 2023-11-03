/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "RestartMode.h"

/** constructor
 *
 */
RestartMode::RestartMode(String name, CommonData_t *common_data)
  : ModeBase(name, common_data) {

} // RestartMode::RestartMode()

/**
 *
 */
bool RestartMode::enter(Mode_t prev_mode) {
  ModeBase::enter(prev_mode);

  log_w("restart ..");

  _cd->msg = " Reboot .. ";
  delay(1000);

  ESP.deepSleep(100);
  delay(100);

  return true;
} // RestartMode::enter()
