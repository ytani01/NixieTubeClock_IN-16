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
  static constexpr char *name = (char *)"Task:NixieTubeArray";

  Task_NixieTubeArray(NixieTubeArray *nta);

protected:
  virtual void setup();
  virtual void loop();

  NixieTubeArray *_nta;
}; // class Task_NixieTubeArray

#endif // _TASK_NIXIETUBEARRAY_H_
