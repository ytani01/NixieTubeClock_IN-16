/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 *
 * simple usage:
```
#include "ConfFile.h"

class ConfA: public ConfFile {
  String data1;
  String data2;

  ConfA(): ConfFile("file") {
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

ConfA *confA;

void setup() {
  confA = new ConfA();
}

void loop() {
  String data1, data2;
  :
  confA->load();
  Serial.println("data1=" + confA->data1);
  Serial.println("data2=" + confA->data2);
  :
  confA->data1 = "abc";
  confA->data2 = "xyz";
  confA->save();
  :
}
```
 */
#ifndef _CONF_FILE_H_
#define _CONF_FILE_H_

#include "esp_spiffs.h"
#include "SPIFFS.h"

/**
 *
 */
class ConfFile {
 public:
  String file_name;

  ConfFile(String file_name);

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
}; // class ConfFile

#endif // _CONF_FILE_H_
