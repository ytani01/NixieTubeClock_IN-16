/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _MODE_H_
#define _MODE_H_

#include <Arduino.h>
#include <esp32-hal-log.h>
#include <map>
#include "commonlib.h"
#include "SysClock.h"
#include "Display.h"
#include "Button.h"
#include "MyRtc.h"

#include "Task_NixieTubeArray.h"
#include "Task_WifiMgr.h"
#include "Task_Ntp.h"

// from main ino
extern std::string VersionString;

extern Display_t *Disp;
extern MyRtc *Rtc;
extern NixieTubeArray *Nxa;

extern Task_NixieTubeArray *TaskNixieTubeArray;
extern Task_WifiMgr *TaskWifiMgr;
extern Task_Ntp *TaskNtp;

extern bool Flag_LoopRunning;
extern bool Flag_ReqModeChange;
extern unsigned long delayOrChangeMode(unsigned long ms);

extern void enableIntr();
extern void disableIntr();

/**
 *
 */
class Mode {
public:
  static std::map<std::string, Mode *> Ent;
  static Mode *Cur;
  static Mode *Prev;

  String name = "";

  bool btn_enable = true;

  Mode();

  virtual void setup();
  virtual void enter();
  virtual void exit();
  virtual void loop();

  virtual void cbBtn(ButtonInfo_t *bi,
                     std::map<std::string, ButtonInfo_t>& btn_info);

  static void add(String name, Mode *mode);
  static void set(String name);
  static void disp_spin(unsigned long interval_ms=100);

protected:
}; // class Mode

#endif // _MODE_H_
