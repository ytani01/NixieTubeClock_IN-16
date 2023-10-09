/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <esp32-hal-log.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/Picopixel.h>
//#include <Fonts/Org_01.h>

#include "commonlib.h"
#include "Task_NetMgr.h"
#include "Task_Ntp.h"
#include "NixieTubeArray.h"

#define VERSION_STR "000101"

typedef RTC_PCF8563 Rtc_t;

typedef enum {
  MODE_MAIN,
  MODE_MENU,
  MODE_RESTART,
  MODE_SCAN_SSID,
  MODE_SET_SSID,
  MODE_N
} Mode_t;

static const char *MODE_T_STR[] = {
  "MAIN",
  "MENU",
  "RESTART",
  "SCAN_SSID",
  "SET_SSID"
};

typedef struct {
  String msg = "";
  Mode_t cur_mode = MODE_MAIN;
  // std::stack<Mode_t> mode_stack; // XXX
  NetMgrInfo_t *netmgr_info;
  Task_NtpInfo_t *ntp_info;
  NixieTubeArray *nta;
  Rtc_t *rtc;
} CommonData_t;

extern CommonData_t commonData;

//-----------------------------
// class Mode {
//  :
//  protected:
//  :
//   CommonData_t *common_data;
//  :
// }
//-----------------------------
#define _cd this->common_data

#endif // _COMMON_H_
