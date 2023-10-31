/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_BOOT_H_
#define _MODE_BOOT_H_

#include "Mode.h"

extern std::string VersionString;

class ModeBoot: public Mode {
 public:
  static const unsigned long BOOT_INTERVAL_MS = 2000;  // ms

  unsigned long interval = BOOT_INTERVAL_MS;
  unsigned long start_ms = 0;

  ModeBoot(unsigned long interval=BOOT_INTERVAL_MS);

  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi, std::map<std::string, bool>& btn_val);
};

#endif // _MODE_BOOT_H_
