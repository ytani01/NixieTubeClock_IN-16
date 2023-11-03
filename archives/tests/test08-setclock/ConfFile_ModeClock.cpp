/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ConfFile_ModeClock.h"

/** constructor
 *
 */
ConfFile_ModeClock::ConfFile_ModeClock()
  : ConfFile(String(ConfFile_ModeClock::FILE_NAME)) {
} // ConfFile_ModeClock::ConfFile_ModeClock()

/** virtual
 *
 * @return number of entries
 */
int ConfFile_ModeClock::load() {
  log_i("");
  if ( this->open_read() < 0 ) {
    return -1;
  }

  std::string data_string = this->read_line().c_str();
  log_v("load|%s|", data_string.c_str());

  if ( data_string.length() > 0 ) {
    this->clock_mode = stoi(data_string);
    log_i("clock_mode = %u", this->clock_mode);
  } else {
    this->clock_mode = 0;
    log_w("clock_mode = %u", this->clock_mode);
  }

  this->close();

  return 1;
} // ConfFile_ModeClock::load()

/** virtual
 * 
 * @return num of entries
 */
int ConfFile_ModeClock::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  std::string data_string = std::to_string(this->clock_mode);
  log_i("|%s|", data_string.c_str());

  this->write_line(String(data_string.c_str()));

  this->close();

  return 1;
} // ConfFile_ModeClock::save()
