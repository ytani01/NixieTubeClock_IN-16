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
  static int INIT_VAL[];

  Task_NixieTubeArray(NixieTubeArray *nta,
                      brightness_t brightness = BRIGHTNESS_RESOLUTION,
                      int *init_val=NULL);

protected:
  virtual void setup();
  virtual void loop();

  NixieTubeArray *_nta;
  int *init_val;
}; // class Task_NixieTubeArray

#endif // _TASK_NIXIETUBEARRAY_H_
