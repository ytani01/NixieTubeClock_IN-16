/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ConfFile_ModeScoreboard.h"

/** constructor
 *
 */
ConfFile_ModeScoreboard::ConfFile_ModeScoreboard()
  : ConfFile(String(ConfFile_ModeScoreboard::FILE_NAME)) {
} // ConfFile_ModeScoreboard::ConfFile_ModeScoreboard()

/** virtual
 *
 * @return number of entries
 */
int ConfFile_ModeScoreboard::load() {
  log_i("");
  if ( this->open_read() < 0 ) {
    return -1;
  }

  std::string out_string = "load|";

  while ( this->file.available() ) {
    for (int i=0; i < this->score.size(); i++) {
      this->score[i] = atoi(read_line().c_str());
      out_string += std::to_string(this->score[i]) + "|";
    } // for(i)
    log_i("%s", out_string.c_str());
  }

  return 3;
} // ConfFile_ModeScoreboard::load()

/** virtual
 * 
 * @return num of entries
 */
int ConfFile_ModeScoreboard::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  for (int i=0; i < this->score.size(); i++) {
    this->write_line(String(this->score[i]));
  } // for(i)

  this->close();

  return 3;
} // ConfFile_ModeScoreboard::save()
