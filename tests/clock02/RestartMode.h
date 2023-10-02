/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _RESTART_MODE_
#define _RESTART_MODE_

#include "Mode.h"

/**
 *
 */
class RestartMode: public Mode {
public:
  RestartMode(String name, CommonData_t *common_data);

  virtual bool enter(Mode_t prev_mode);
}; // class RestartMode

#endif // _RESTART_MODE_
