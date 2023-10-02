/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "ConfSsid.h"

/** constructor
 *
 */
ConfSsid::ConfSsid()
  : ConfBase(String(ConfSsid::FILE_NAME)) {

} // ConfSsid::ConfSsid()

/** virtual
 *
 */
int ConfSsid::load() {
  if ( this->open_read() < 0 ) {
    return -1;
  }
  this->ent.clear();

  while ( this->file.available() ) {
    String ssid = this->read_line();
    String pw = this->read_line();
    log_d("load|%s|%s|", ssid.c_str(), pw.c_str());

    this->ent[ssid.c_str()] = pw.c_str();
  } // while(this->file.available())

  this->close();

  for (auto it=this->ent.begin(); it != ent.end(); it++) {
    log_i("ent|%s|%s|", (it->first).c_str(), (it->second).c_str());
  }
  return this->line_count;
} // ConfSsid::load()

/** virtual
 *
 */
int ConfSsid::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  for (auto it=this->ent.begin(); it != ent.end(); it++) {
    String ssid = (it->first).c_str();
    String pw = (it->second).c_str();
    if ( pw.length() == 0 ) {
      log_w("|%s|%s| .. ignored", ssid.c_str(), pw.c_str());
      continue;
    }
    log_i("|%s|%s|", ssid.c_str(), pw.c_str());
    
    this->write_line(ssid);
    this->write_line(pw);
  }
  this->close();

  return this->line_count;
} // ConfSsid::save()
