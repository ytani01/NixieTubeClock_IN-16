/**
 * (c) 2021 Yoichi Tanibayashi
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
  const char* myname = "ConfWifi::load";

  disableIntr();

  if(!SPIFFS.begin(true)) {
    Serial.printf("%s> ERROR: SPIFFS mout failed: %s", myname, config_file);
    enableIntr();
    return -1;
  }

  File file = SPIFFS.open(config_file, "r");
  if (!file) {
    Serial.printf("%s> %s: open failed\n", myname, config_file);
    this->ssid = "";
    this->ssid_pw = "";
    enableIntr();
    return -1;
  }

  this->ssid = this->read_line(file);
  Serial.printf("%s> SSID: %s\n", myname, this->ssid.c_str());
  
  this->ssid_pw = this->read_line(file);
  Serial.printf("%s> SSID PW: %s\n", myname, this->ssid_pw.c_str());

  file.close();
  enableIntr();
  return 0;
} // ConfWifi::load

/**
 *
 */
int ConfWifi::save(const char* config_file) {
  const char* myname = "ConfWifi::save";

  this->ssid.trim();
  this->ssid_pw.trim();

  Serial.printf("%s> SSID=%s\n", myname, ssid.c_str());
  Serial.printf("%s> SSID PW=%s\n", myname, ssid_pw.c_str());

  disableIntr();

  File file = SPIFFS.open(config_file, "w");
  if (!file) {
    Serial.printf("%s> ERROR open failed: %s\n", myname, config_file);
    enableIntr();
    return -1;
  }
  
  file.println(this->ssid);
  file.println(this->ssid_pw);
  file.close();
  enableIntr();

  Serial.printf("%s> wrote: %s\n", myname, config_file);
  delay(100);
  return 0;
} // ConfWifi::save()

/**
 *
 */
void ConfWifi::print() {
  Serial.printf("SSID: %s\n", this->ssid.c_str());
  Serial.printf("SSID PW: %s\n", this->ssid_pw.c_str());
} // ConfWifi::print()
