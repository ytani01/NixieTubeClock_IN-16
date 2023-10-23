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
 * @return number of entries
 */
int ConfFile_Ssid::load() {
  log_i("");
  if ( this->open_read() < 0 ) {
    return -1;
  }

  this->ent.clear();

  while ( this->file.available() ) {
    std::string ssid = this->read_line().c_str();
    std::string pw = this->read_line().c_str();
    log_v("load|%s|%s|", ssid.c_str(), pw.c_str());

    this->ent[ssid] = pw;
  } // while(this->file.available())

  this->close();

  for (auto ent: this->ent) {
    log_i("ent|%s|%s|", ent.first.c_str(), ent.second.c_str());
  }
  return this->ent.size();
} // ConfFile_Ssid::load()

/** virtual
 * 
 * @return num of entries
 */
int ConfFile_Ssid::save() {
  if ( this->open_write() < 0 ) {
    return -1;
  }

  for (auto ent: this->ent ) {
    std::string ssid = ent.first.c_str();
    std::string pw = ent.second.c_str();
    if ( pw.length() == 0 ) {
      log_w("|%s|%s| .. ignored", ssid.c_str(), pw.c_str());
      continue;
    }
    log_i("|%s|%s|", ssid.c_str(), pw.c_str());
    
    this->write_line(String(ssid.c_str()));
    this->write_line(String(pw.c_str()));
  }
  this->close();

  return this->ent.size();
} // ConfFile_Ssid::save()
