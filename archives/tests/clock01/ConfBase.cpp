/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "ConfBase.h"

/** constructor
 *
 */
ConfBase::ConfBase(String file_name) {
  this->file_name = file_name;

  if ( ! SPIFFS.begin(true) ) {
    log_e("%s: SPIFFS mount failed", this->file_name.c_str());
  }
} // ConfBase::ConfBase()

/** virtual
 *
 */
int ConfBase::load() {
  // test code
  if ( ! this->open_read() ) {
    return -1;
  }

  while ( int fsize = this->file.available() ) {
    String line = this->read_line();
    log_i("%d: %d: \"%s\"", this->line_count, fsize, line.c_str());
  }
  this->file.close();

  return this->line_count;
} // ConfBase::load()

/** virtual
 *
 */
int ConfBase::save() {
  // test code
  if ( ! this->open_write() ) {
    return -1;
  }

  this->write_line("test1");
  this->write_line("test2");
  this->write_line("test3");
  this->file.close();
  
  return this->line_count;
} // ConfBase::save()
  
/** protected
 *
 */
bool ConfBase::open_read() {
  log_i("%s", this->file_name.c_str());

  this->file = SPIFFS.open(this->file_name, "r");
  if ( ! this->file ) {
    log_e("%s: open failed", file_name.c_str());
    return false;
  }
  this->line_count = 0;
  return true;
} // ConfBase::open_read()

/** protected
 *
 */
bool ConfBase::open_write() {
  log_i("%s", this->file_name.c_str());

  this->file = SPIFFS.open(this->file_name, "w");
  if ( ! this->file ) {
    log_e("%s: open failed", file_name.c_str());
    return false;
  }
  this->line_count = 0;
  return true;
} // ConfBase::open_write()

/** protected
 *
 */
void ConfBase::close() {
  this->file.close();
} // ConfBase::close()

/** protected
 *
 */
String ConfBase::read_line() {
  int ret = this->file.available();
  if ( ret <= 0 ) {
    log_w("ret=%d: EOF?", ret);
    return "";
  }

  String line = this->file.readStringUntil('\n');
  line.trim(); // XXX Important !!
  this->line_count++;
  log_i("%5d|%s|", line_count, line.c_str());
  return line;
} // ConfBase::read_line()

/** protected
 *
 */
String ConfBase::write_line(String line) {
  line.trim(); // XXX Important !!
  this->file.println(line);
  this->line_count++;
  log_i("%5d|%s|", line_count, line.c_str());
  return line;
} // ConfBase::write_line()
