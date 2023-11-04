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

  std::string clock_mode_string = "";
  std::string effect_i_string = "";
  std::string demo_mode_string = "";

  while ( this->file.available() ) {
    clock_mode_string = this->read_line().c_str();
    effect_i_string = this->read_line().c_str();
    demo_mode_string = this->read_line().c_str();
    log_d("load|%s|%s|%s|",
          data_string.c_str(),
          effect_i_string.c_str(),
          demo_mode_string.c_str());
  }

  this->close();

  if ( clock_mode_string.length() > 0 ) {
    this->clock_mode = stoi(clock_mode_string);
    log_i("clock_mode = %u", this->clock_mode);
  }

  if ( effect_i_string.length() > 0 ) {
    this->effect_i = stoi(effect_i_string);
    log_i("effect_i = %u", this->effect_i);
  }

  if ( demo_mode_string.length() > 0 ) {
    if ( demo_mode_string == "true" ) {
      this->demo_mode = true;
    } else {
      this->demo_mode = false;
    }
    log_i("demo_mode = %s", this->demo_mode ? "true" : "false");
  }

  return 2;
} // ConfFile_ModeClock::load()

/** virtual
 * 
 * @return num of entries
 */
int ConfFile_ModeClock::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  std::string clock_mode_string = std::to_string(this->clock_mode);
  std::string effect_i_string = std::to_string(this->effect_i);
  std::string demo_mode_string = this->demo_mode ? "true" : "false";
  log_i("|%s|%s|%s|",
        clock_mode_string.c_str(),
        effect_i_string.c_str(),
        demo_mode_string.c_str());

  this->write_line(String(clock_mode_string.c_str()));
  this->write_line(String(effect_i_string.c_str()));
  this->write_line(String(demo_mode_string.c_str()));

  this->close();

  return 2;
} // ConfFile_ModeClock::save()
