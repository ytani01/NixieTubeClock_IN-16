/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 *
 * simple usage:
```
#include <esp32-hal-log.h>
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
  log_i("data1=%s", conf->data1.c_str());
  log_i("data2=%s", conf->data2.c_str());
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
  
  virtual bool remove();

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
