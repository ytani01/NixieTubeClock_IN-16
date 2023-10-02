/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef CONF_WIFI_H
#define CONF_WIFI_H

#include "SPIFFS.h"

static const char WIFI_FILE[] = "/wifi.txt";

/**
 *
 */
class ConfWifi {
 public:

  String ssid = "";
  String ssid_pw = "";
  String ntp_svr[3];

  ConfWifi();

  int load(const char* config_file = WIFI_FILE);
  int save(const char* config_file = WIFI_FILE);

  String read_line(File file);

  void print();
};

#endif // CONF_WIFI_H
