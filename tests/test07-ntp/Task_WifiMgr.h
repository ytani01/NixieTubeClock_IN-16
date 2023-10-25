/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_WIFI_MGR_H_
#define _TASK_WIFI_MGR_H_

#include "Task.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "commonlib.h"
#include "ConfFile_Ssid.h"

/**
 *
 */
typedef enum {
  WIFI_MGR_MODE_STA,
  WIFI_MGR_MODE_AP,
  WIFI_MGR_MODE_NONE
} wifi_mgr_mode_t;

static const char *WIFI_MGR_MODE_T_STR[] = {
  "STA",
  "AP",
  "NONE"
};

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

#define WL_STATUS_T_STR2(s) (s <= WL_DISCONNECTED ? WL_STATUS_T_STR[s] : std::to_string(s).c_str())

/**
 *
 */
class Task_WifiMgr: public Task {
 public:
  static const int STA_RETRY_MAX = 5;
  static constexpr char* WEB_NAME = (char *)"WiFi Manager";

  static const int DNS_PORT = 53;
  static const int WEB_PORT = 80;
  
  static ConfFile_Ssid *Obj_ConfFile_Ssid;
  static WiFiMulti *Obj_WiFiMulti;

  static WiFiEvent_t LastEvId;
  static char LastEvStr[];
  static WiFiEventInfo_t LastEvInfo;

  wifi_mgr_mode_t mode = WIFI_MGR_MODE_STA;
  wl_status_t wl_stat;

  std::string cur_ssid = "";

  std::string ap_ssid;
  unsigned long wifi_try_count;

  Task_WifiMgr(std::string ap_ssid_hdr);

 protected:
  IPAddress ap_ip, ap_netmask;
  DNSServer dns_svr;
  static WebServer web_svr;
  
  virtual void setup();
  virtual void loop();

  void loop_sta_mode();
  void loop_ap_mode();
  
  static void on_wifi_event(WiFiEvent_t ev_id, WiFiEventInfo_t ev_info);

  static std::string html_header(std::string title);
  static std::string html_footer();

  static void handle_top();
  static void handle_select_ssid();
  static void handle_save_ssid();
  static void handle_do_scan();
  static void handle_confirm_reboot();
  static void handle_do_reboot();
  static void handle_not_found();
}; // class Task_WifiMgr
#endif // _TASK_WIFI_MGR_H_
