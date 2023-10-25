/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_NIXIETUBEARRAY_H_
#define _TASK_NIXIETUBEARRAY_H_

#include "Task.h"
#include "Nixie.h"
#include "NixieTubeArray.h"

/**
 *
 */
class Task_NixieTubeArray
  : public Task {
public:
  static const uint32_t STACK_SIZE = 4 * 1024;
  static const UBaseType_t PRIORITY = 4;
  static const UBaseType_t CORE = APP_CPU_NUM;

  static int INIT_VAL[];

  Task_NixieTubeArray(NixieTubeArray *nta,
                      brightness_t brightness = BRIGHTNESS_RESOLUTION,
                      int *init_val=NULL,
                      uint32_t stack_size=STACK_SIZE,
                      UBaseType_t priority=PRIORITY,
                      UBaseType_t core=CORE);

protected:
  virtual void setup();
  virtual void loop();

  NixieTubeArray *_nta;
  int *init_val;
}; // class Task_NixieTubeArray

#endif // _TASK_NIXIETUBEARRAY_H_
