/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeB.h"

void ModeB::loop() {
  log_d("name: %s", this->name.c_str());
  delay(1000);
}
