/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _CONF_SSID_H_
#define _CONF_SSID_H_

#include <unordered_map>
#include "ConfBase.h"

/**
 *
 */
class ConfSsid: public ConfBase {
 public:
  static constexpr char *FILE_NAME = (char *)"/ssid";

  std::unordered_map<std::string, std::string> ent;

  ConfSsid();
  virtual int load();
  virtual int save();
}; // class ConfSsid

#endif // _CONF_SSID_H_
