/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <Arduino.h>
#include <vector>
#include <esp_system.h>
#include <esp32-hal-log.h>

#include "commonlib.h"
//#include "NetMgr.h"
//#include "Task_Ntp.h"
#include "Display.h"

#define VERSION_STR "0.02"

/*
typedef struct {
  NetMgrMode_t mode;
  uint8_t mac_addr[6];
  IPAddress ip_addr;
  String new_ssid;
  String ssid;
  String ap_ssid;
} NetMgrInfo_t;
*/

typedef struct {
  //  NetMgr* net_mgr;
  //  Task_Ntp* ntp;
  Display_t* disp;
} CommonData_t;

extern CommonData_t commonData;
#define Disp commonData.disp

#endif // _COMMON_H_
