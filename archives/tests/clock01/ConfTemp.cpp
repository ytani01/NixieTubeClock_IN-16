/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "ConfTemp.h"

/** constructor
 *
 */
ConfTemp::ConfTemp()
  : ConfBase(String(ConfTemp::FILE_NAME)) {

} // ConfTemp::ConfTemp()

/** virtual
 *
 */
int ConfTemp::load() {
  if ( ! this->open_read() ) {
    this->temp_offset = 0.0;
    log_e("%s: open failed, temp_offset=%.1f",
          this->file.name(), this->temp_offset);
    return -1;
  }

  String line = this->read_line();
  this->close();

  log_i("line=%s", line.c_str());
  if ( line == "" ) {
    this->temp_offset = 0.0;
    this->save();
  } else {
    this->temp_offset = line.toFloat();
  }
  log_i("temp_offset=%f", this->temp_offset);
  
  return this->line_count;
} // ConfTemp::load()

/** virtual
 *
 */
int ConfTemp::save() {
  if ( ! this->open_write() ) {
    log_e("%s: open faild", this->file.name());
    return -1;
  }

  String line = String(this->temp_offset);
  this->write_line(line);
  this->close();

  return this->line_count;
} // ConfTemp::save()
