/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Restart.h"

/** constructor
 *
 */
Mode_Restart::Mode_Restart(String name, CommonData_t *common_data)
  : Mode(name, common_data) {

} // Mode_Restart::Mode_Restart()

/**
 *
 */
bool Mode_Restart::enter(Mode_t prev_mode) {
  Mode::enter(prev_mode);

  log_w("restart ..");

  _cd->msg = " Reboot .. ";
  delay(1000);

  ESP.deepSleep(100);
  delay(100);

  return true;
} // Mode_Restart::enter()
