/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "ConfFps.h"

/** constructor
 *
 */
ConfFps::ConfFps()
  : ConfBase(String(ConfFps::FILE_NAME)) {
} // ConfFps::ConfFps()

/** virtual
 *
 */
int ConfFps::load() {
  if ( this->open_read() < 0 ) {
    this->disp_fps = true;
    this->save();
    return -1;
  }

  String line = this->read_line();
  if ( line == "true" ) {
    this->disp_fps = true;
  } else {
    this->disp_fps = false;
  }
  this->close();

  if ( line == "" ) {
    this->save();
  }
  
  return this->line_count;
} // ConfFps::load()

/** virtual
 *
 */
int ConfFps::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  if ( this->disp_fps ) {
    this->write_line("true");
  } else {
    this->write_line("false");
  }
  this->close();

  return this->line_count;
} // ConfFps::save()
