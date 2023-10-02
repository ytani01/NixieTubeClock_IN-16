/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ConfWifi.h"

extern void enableIntr();
extern void disableIntr();

/**
 *
 */
ConfWifi::ConfWifi() {
} // ConfWifi::ConfWifi()

/**
 *
 */
String ConfWifi::read_line(File file) {
  String line = file.readStringUntil('\n');
  line.trim();
  return line;
}

/**
 *
 */
int ConfWifi::load(const char* config_file) {
  disableIntr();

  if(!SPIFFS.begin(true)) {
    log_e("ERROR: SPIFFS mout failed: %s", config_file);
    enableIntr();
    return -1;
  }

  File file = SPIFFS.open(config_file, "r");
  if (!file) {
    log_i("%s: open failed", config_file);
    this->ssid = "";
    this->ssid_pw = "";
    enableIntr();
    return -1;
  }

  this->ssid = this->read_line(file);
  log_i("SSID: %s", this->ssid.c_str());
  
  this->ssid_pw = this->read_line(file);
  log_i("SSID PW: %s", this->ssid_pw.c_str());

  file.close();
  enableIntr();
  return 0;
} // ConfWifi::load

/**
 *
 */
int ConfWifi::save(const char* config_file) {
  this->ssid.trim();
  this->ssid_pw.trim();

  log_i("SSID=%s", ssid.c_str());
  log_i("SSID PW=%s", ssid_pw.c_str());

  disableIntr();

  File file = SPIFFS.open(config_file, "w");
  if (!file) {
    log_e("ERROR open failed: %s", config_file);
    enableIntr();
    return -1;
  }
  
  file.println(this->ssid);
  file.println(this->ssid_pw);
  file.close();
  enableIntr();

  log_i("wrote: %s", config_file);
  delay(100);
  return 0;
} // ConfWifi::save()

/**
 *
 */
void ConfWifi::print() {
  log_i("SSID: %s", this->ssid.c_str());
  log_i("SSID PW: %s", this->ssid_pw.c_str());
} // ConfWifi::print()
