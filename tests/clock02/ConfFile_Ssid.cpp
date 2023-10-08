/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ConfFile_Ssid.h"

/** constructor
 *
 */
ConfFile_Ssid::ConfFile_Ssid()
  : ConfFile(String(ConfFile_Ssid::FILE_NAME)) {
} // ConfFile_Ssid::ConfFile_Ssid()

/** virtual
 *
 */
int ConfFile_Ssid::load() {
  log_i("");
  if ( this->open_read() < 0 ) {
    return -1;
  }
  this->ent.clear();

  while ( this->file.available() ) {
    String ssid = this->read_line();
    String pw = this->read_line();
    log_i("load|%s|%s|", ssid.c_str(), pw.c_str());

    this->ent[ssid.c_str()] = pw.c_str();
  } // while(this->file.available())

  this->close();

  for (auto it=this->ent.begin(); it != ent.end(); it++) {
    log_i("ent|%s|%s|", (it->first).c_str(), (it->second).c_str());
  }
  return this->line_count;
} // ConfFile_Ssid::load()

/** virtual
 *
 */
int ConfFile_Ssid::save() {
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
} // ConfFile_Ssid::save()
