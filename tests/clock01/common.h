/**
 * $Id: 1f0fd03a1ae0002a569a4fb77a967acf2cd615c4 $
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <esp32-hal-log.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/Picopixel.h>
//#include <Fonts/Org_01.h>

#include "NetMgrTask.h"
#include "NtpTask.h"

#define VERSION_STR "0.xx"

typedef enum {
              MODE_MAIN,
              MODE_MENU,
              MODE_RESTART,
              MODE_SET_TEMP_OFFSET,
              MODE_SCAN_SSID,
              MODE_SET_SSID,
              MODE_N
} Mode_t;
static const char *MODE_T_STR[] =
  {"MAIN", "MENU", "RESTART", "SET_TEMP_OFFSET", "SCAN_SSID", "SET_SSID"};

typedef struct {
  String msg;
  Mode_t cur_mode;
  // std::stack<Mode_t> mode_stack; // XXX
  NetMgrInfo_t *netmgr_info;
  NtpTaskInfo_t *ntp_info;
} CommonData_t;

#define _cd this->common_data

#endif // _COMMON_H_
