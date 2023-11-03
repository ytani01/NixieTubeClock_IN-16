/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _CONFFILE_SSID_H_
#define _CONFFILE_SSID_H_

#include <unordered_map>
#include "ConfFile.h"

/**
 *
 */
class ConfFile_Ssid: public ConfFile {
 public:
  static constexpr char *FILE_NAME = (char *)"/ssid";

  std::unordered_map<std::string, std::string> ent;
  //std::unordered_map<String, String> ent2;

  ConfFile_Ssid();
  virtual int load();
  virtual int save();
}; // class ConfFile_Ssid

#endif // _CONFFILE_SSID_H_
