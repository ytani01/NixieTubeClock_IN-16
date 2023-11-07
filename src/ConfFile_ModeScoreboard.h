/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _CONFFILE_MODE_SCOREBOARD_H_
#define _CONFFILE_MODE_SCOREBOARD_H_

#include "ConfFile.h"

/**
 *
 */
class ConfFile_ModeScoreboard: public ConfFile {
 public:
  static constexpr char *FILE_NAME = (char *)"/mode_scorebord";

  std::vector<int> score = {0, 0, 0};

  ConfFile_ModeScoreboard();
  virtual int load();
  virtual int save();
}; // class ConfFile_ModeScoreboard

#endif // _CONFFILE_MODE_SCOREBOARD_H_
