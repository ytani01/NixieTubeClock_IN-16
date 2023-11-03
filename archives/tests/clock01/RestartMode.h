/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _RESTART_MODE_
#define _RESTART_MODE_

#include "ModeBase.h"

/**
 *
 */
class RestartMode: public ModeBase {
 public:
  RestartMode(String name, CommonData_t *common_data);

  virtual bool enter(Mode_t prev_mode);
}; // class RestartMode

#endif // _RESTART_MODE_
