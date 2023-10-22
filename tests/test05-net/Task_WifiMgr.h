/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_WIFI_MGR_H_
#define _TASK_WIFI_MGR_H_

#include "Task.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include "ConfFile_Ssid.h"

/**
 *
 */
typedef enum {
  WIFI_MGR_MODE_STA,
  WIFI_MGR_MODE_AP,
  WIFI_MGR_MODE_SIZE
} wifi_mgr_mode_t;

static const char *WL_STATUS_T_STR[] = {
  "IDLE_STATUS",
  "NO_SSID_AVAIL",
  "SCAN_COMPLETED",
  "CONNECTED",
  "CONNECT_FAILED",
  "CONNECTION_LOST",
  "DISCONNECTED",
  "WL_STATUS_SIZE"
};

/**
 *
 */
class Task_WifiMgr: public Task {
 public:
  static constexpr char *NAME = (char *)"Task_WifiMgr";
  static const int STA_RETRY_MAX = 10;
  
  static ConfFile_Ssid *Obj_ConfFile_Ssid;
  static WiFiMulti *Obj_WiFiMulti;

  static WiFiEvent_t LastEvId;
  static WiFiEventInfo_t LastEvInfo;

  wifi_mgr_mode_t mode = WIFI_MGR_MODE_STA;
  std::string ap_ssid_hdr;
  unsigned long wifi_try_count;

  Task_WifiMgr(std::string ap_ssid_hdr);

 protected:
  virtual void setup();
  virtual void loop();

  void loop_sta_mode();
  void loop_ap_mode();
  
  static void on_wifi_event(WiFiEvent_t ev_id, WiFiEventInfo_t ev_info);
}; // class Task_WifiMgr
#endif // _TASK_WIFI_MGR_H_
