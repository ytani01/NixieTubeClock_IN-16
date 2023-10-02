/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _CONF_FPS_H_
#define _CONF_FPS_H_

#include "ConfBase.h"

/**
 *
 */
class ConfFps: public ConfBase {
 public:
  static constexpr char *FILE_NAME = (char *)"/fps";

  bool disp_fps;

  ConfFps();
  virtual int load();
  virtual int save();
}; // class ConfFps

#endif // _CONF_FPS_H_
