/**
 * Copyright (c) 2021 Yoichi Tanibayashi
 */
#ifndef _NETMGR_H_
#define _NETMGR_H_

#include <esp_wifi.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "commonlib.h"
#include "SSIDent.h"
#include "ConfSsid.h"

typedef enum { NETMGR_MODE_NULL,
               NETMGR_MODE_START,
               NETMGR_MODE_TRY_WIFI,
               NETMGR_MODE_AP_INIT,
               NETMGR_MODE_AP_LOOP,
               NETMGR_MODE_SCANNING_SSID,
               NETMGR_MODE_WIFI_ON,
               NETMGR_MODE_WIFI_OFF,
               NETMGR_MODE_N
} NetMgrMode_t;
static const char *NETMGR_MODE_STR[]
= {"NULL", "START", "TRY_WIFI", "AP_INIT", "AP_LOOP",
   "SCANNING_SSID", "WIFI_ON", "WIFI_OFF"};

static const char *WL_STATUS_T_STR[]
= {"IDLE_STATUS", "NO_SSID_AVAIL", "SCAN_COMPLETED", "CONNECTED",
   "CONNECT_FAILED", "CONNECTION_LOST", "DISCONNECTED"};

/**
 *
 */
class NetMgr {
public:
  static const unsigned int TRY_INTERVAL  = 1000; // ms
  static const unsigned int DEF_TRY_COUNT_MAX = 8;
  
  static const unsigned int SSID_N_MAX = 40;
  
  static const int DNS_PORT    = 53;
  static const int WEBSVR_PORT = 80;

  NetMgrMode_t cur_mode = NETMGR_MODE_START;

  static String myName;
  static int16_t ssidN;
  static SSIDent ssidEnt[SSID_N_MAX];

  unsigned int try_count_max = DEF_TRY_COUNT_MAX;

  uint8_t mac_addr[6];
  String cur_ssid = "";
  IPAddress ip_addr;

  boolean net_is_available = false;;

  String ap_ssid_hdr = "AP_";
  String ap_ssid = ap_ssid_hdr;
  String ap_ssid_pw  = "xxxxxxxx"; // unused

  int ap_ip_int[4]      = {192,168,111,1};
  int ap_netmask_int[4] = {255,255,255,0};

  IPAddress ap_ip;       // initialize in constructor
  IPAddress ap_netmask;  // initialize in constructor
  DNSServer dns_svr;

  NetMgr(String ap_ssid_hdr, unsigned int try_count_max=DEF_TRY_COUNT_MAX);
  NetMgrMode_t loop();

  void save_ssid(String ssid, String pw);
  void restart();
  //  void set_ssid(String ssid="");
  
protected:
  static WebServer web_svr;
 
  unsigned int _loop_count = 0;
  bool restart_flag = false;

  void _restart();

  static void         async_scan_ssid_start();
  static unsigned int async_scan_ssid_wait();

  static String html_header(String title);
  static String html_footer();

  static void handle_top();
  static void handle_select_ssid();
  static void handle_save_ssid();
  static void handle_do_scan();
  static void handle_confirm_reboot();
  static void handle_do_reboot();
};

#endif // _NETMGR_H_
