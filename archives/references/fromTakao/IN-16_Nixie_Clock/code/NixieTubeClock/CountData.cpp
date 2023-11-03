/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "CountData.h"

extern void enableIntr();
extern void disableIntr();

/**
 *
 */
CountData::CountData() {
} // CountData::CountData()

/**
 *
 */
String CountData::read_line(File file) {
  String line = file.readStringUntil('\n');
  line.trim();
  return line;
}

/**
 *
 */
int CountData::load(const char* config_file) {
  const char* myname = "CountData::load";

  disableIntr();

  if(!SPIFFS.begin(true)) {
    Serial.printf("%s> ERROR: SPIFFS mout failed: %s", myname, config_file);
    enableIntr();
    return -1;
  }

  File file = SPIFFS.open(config_file, "r");
  if (!file) {
    Serial.printf("%s> %s: open failed\n", myname, config_file);

    enableIntr();
    return -1;
  }

  for (int i=0; i < COUNT_N; i++) {
    this->count[i] = this->read_line(file).toInt();
    Serial.printf("%s> count[%d]=%d\n", myname, i, this->count[i]);
  } // for(i)

  file.close();
  enableIntr();
  return 0;
} // CountData::load

/**
 *
 */
int CountData::save(const char* config_file) {
  const char* myname = "CountData::save";

  disableIntr();

  for (int i=0; i < COUNT_N; i++) {
    Serial.printf("%s> data[%d]=%d\n", myname, i, this->count[i]);
  } // for(i)

  File file = SPIFFS.open(config_file, "w");
  if (!file) {
    Serial.printf("%s> ERROR open failed: %s\n", myname, config_file);
    enableIntr();
    return -1;
  }
  
  for (int i=0; i < COUNT_N; i++) {
    file.printf("%d\n", this->count[i]);
  } // for(i)

  file.close();
  enableIntr();

  Serial.printf("%s> wrote: %s\n", myname, config_file);
  delay(1);
  return 0;
} // CountData::save()

/**
 *
 */
void CountData::print() {
  //Serial.printf("SSID: %s\n", this->ssid.c_str());
  //Serial.printf("SSID PW: %s\n", this->ssid_pw.c_str());
} // CountData::print()
