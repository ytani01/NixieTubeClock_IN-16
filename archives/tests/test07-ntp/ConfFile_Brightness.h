/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _CONFFILE_BRIGHTNESS_H_
#define _CONFFILE_BRIGHTNESS_H_

#include "ConfFile.h"
#include "Nixie.h"

/**
 *
 */
class ConfFile_Brightness: public ConfFile {
 public:
  static constexpr char *FILE_NAME = (char *)"/brightness";

  brightness_t brightness = BRIGHTNESS_RESOLUTION;

  ConfFile_Brightness();
  virtual int load();
  virtual int save();
}; // class ConfFile_Brightness

#endif // _CONFFILE_BRIGHTNESS_H_
