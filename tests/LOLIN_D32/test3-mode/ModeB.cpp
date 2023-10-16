/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "ModeB.h"

void ModeB::loop() {
  log_d("%s: name: %s",
        SysClock::now_str(),
        this->name.c_str());
  delay(5000);
}
