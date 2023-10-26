/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ConfFile_Brightness.h"

/** constructor
 *
 */
ConfFile_Brightness::ConfFile_Brightness()
  : ConfFile(String(ConfFile_Brightness::FILE_NAME)) {
} // ConfFile_Brightness::ConfFile_Brightness()

/** virtual
 *
 * @return number of entries
 */
int ConfFile_Brightness::load() {
  log_i("");
  if ( this->open_read() < 0 ) {
    return -1;
  }

  std::string bri_string = this->read_line().c_str();
  log_v("load|%s|", bri_string.c_str());

  if ( bri_string.length() > 0 ) {
    this->brightness = (brightness_t)stoi(bri_string);
    log_i("brightness = %u", this->brightness);
  } else {
    this->brightness = 0;
    log_w("brightness = %u", this->brightness);
  }

  this->close();

  return 1;
} // ConfFile_Brightness::load()

/** virtual
 * 
 * @return num of entries
 */
int ConfFile_Brightness::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  std::string bri_string = std::to_string(this->brightness);
  log_i("|%s|", bri_string.c_str());

  this->write_line(String(bri_string.c_str()));

  this->close();

  return 1;
} // ConfFile_Brightness::save()
