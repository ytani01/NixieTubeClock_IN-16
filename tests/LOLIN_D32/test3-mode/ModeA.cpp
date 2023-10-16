/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeA.h"

ModeA::ModeA(): Mode() {
}

void ModeA::loop() {
  log_d("class %s: name: %s", __CLASS_NAME__.c_str(), this->name.c_str());
  delay(1000);
}
