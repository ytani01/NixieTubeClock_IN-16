/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _CONFFILE_MODE_CLOCK_H_
#define _CONFFILE_MODE_CLOCK_H_

#include "ConfFile.h"
#include "Nixie.h"

/**
 *
 */
class ConfFile_ModeClock: public ConfFile {
 public:
  static constexpr char *FILE_NAME = (char *)"/mode_clock";

  int clock_mode = 0;

  ConfFile_ModeClock();
  virtual int load();
  virtual int save();
}; // class ConfFile_ModeClock

#endif // _CONFFILE_MODE_CLOCK_H_
