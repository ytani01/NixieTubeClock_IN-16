/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _CONF_TEMP_H_
#define _CONF_TEMP_H_

#include "ConfBase.h"

/**
 *
 */
class ConfTemp: public ConfBase {
 public:
  static constexpr char *FILE_NAME = (char *)"/temp.conf";

  float temp_offset;

  ConfTemp();
  virtual int load();
  virtual int save();
}; // class ConfTemp

#endif // _CONF_TEMP_H_
