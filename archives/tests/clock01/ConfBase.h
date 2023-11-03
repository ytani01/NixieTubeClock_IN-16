/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 *
 * simple usage:
```
#include "ConfBase.h"

class ConfA: public ConfBase {
  String data1;
  String data2;

  ConfA(): ConfBase("file") {
  };

  virtual int load() {
    if ( ! this->open_read() ) {
      this->data1 = "";
      this->data2 = "";
      return 0;
    }
    this->data1 = this->read_line();
    this->data2 = this->read_line();
    this->close();
    return this->line_count;
  };

  virtual int save() {
    if ( ! this->open_write() ) {
      return 0;
    }
    this->write_line(this->data1);
    this->write_line(this->data2);
    this->close();
    return this->line_count;
  };
};

ConfA *conf;

void setup() {
  conf = new ConfA();
}

void loop() {
  String data1, data2;
  :
  conf->load();
  Serial.println("data1=" + conf->data1);
  Serial.println("data2=" + conf->data2);
  :
  conf->data1 = "abc";
  conf->data2 = "xyz";
  conf->save();
  :
}
```
 */
#ifndef _CONF_BASE_H_
#define _CONF_BASE_H_

#include "esp_spiffs.h"
#include "SPIFFS.h"

/**
 *
 */
class ConfBase {
 public:
  String file_name;

  ConfBase(String file_name);

  virtual int load();
  virtual int save();
  
 protected:
  File file;
  int line_count;

  bool open_read();
  bool open_write();
  void close();
  String read_line();
  String write_line(String line);
}; // class ConfBase

#endif // _CONF_BASE_H_
