/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_RESTART_
#define _MODE_RESTART_

#include "Mode.h"

/**
 *
 */
class Mode_Restart: public Mode {
public:
  Mode_Restart(String name, CommonData_t *common_data);

  virtual bool enter(Mode_t prev_mode);
}; // class Mode_Restart

#endif // _MODE_RESTART_
