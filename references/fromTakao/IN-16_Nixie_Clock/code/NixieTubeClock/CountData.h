/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef COUNT_DATA_H
#define COUNT_DATA_H

#include "SPIFFS.h"

static const char COUNT_FILE[] = "/count.txt";

/**
 *
 */
class CountData {
 public:
  static const int COUNT_N = 3;

  int count[COUNT_N];

  CountData();

  int load(const char* config_file=COUNT_FILE);
  int save(const char* config_file=COUNT_FILE);

  String read_line(File file);

  void print();
};

#endif // COUNT_DATA_H
