/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef NETMGR_H
#define NETMGR_H

#include <esp_system.h>
#include <esp32-hal-log.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "SSIDent.h"
#include "ConfWifi.h"

/**
 *
 */
class NetMgr {
 public:
  static const unsigned int WIFI_TRY_INTERVAL  = 500; // ms
  static const unsigned int WIFI_TRY_COUNT_MAX = 15;

  static const unsigned int SSID_N_MAX = 30;

  static const int DNS_PORT    = 53;
  static const int WEBSVR_PORT = 80;

  static const mode_t MODE_NULL          = 0x00;
  static const mode_t MODE_START         = 0x01;
  static const mode_t MODE_TRY_WIFI      = 0x02;
  static const mode_t MODE_AP_INIT       = 0x10;
  static const mode_t MODE_AP_LOOP       = 0x11;
  static const mode_t MODE_SCANNING_SSID = 0x20;
  static const mode_t MODE_WIFI_ON       = 0xf0;
  static const mode_t MODE_WIFI_OFF      = 0xf1;

  static String myName;
  static unsigned int ssidN;
  static SSIDent ssidEnt[SSID_N_MAX];

  boolean net_is_available = false;;
  mode_t cur_mode   = MODE_START;

  String    ap_ssid_hdr = "NIXIE_CLOCK_";
  String    ap_ssid     = ap_ssid_hdr;
  String    ap_ssid_pw  = "xxxxxxxx"; // unused

  int       ap_ip_int[4]      = {192,168,100,1};
  int       ap_netmask_int[4] = {255,255,255,0};

  IPAddress ap_ip;       // initialize in constructor
  IPAddress ap_netmask;  // initialize in constructor
  DNSServer dns_svr;

  NetMgr();
  mode_t loop();

 private:
  unsigned int _loop_count = 0;

  static WebServer web_svr;

  static unsigned int scan_ssid(SSIDent ssid_ent[]);
  static void         async_scan_ssid_start();
  static unsigned int async_scan_ssid_wait(SSIDent ssid_ent[]);

  static String html_header(String title);
  static String html_footer();

  static void handle_top();
  static void handle_select_ssid();
  static void handle_save_ssid();
  static void handle_do_scan();
  static void handle_confirm_reboot();
  static void handle_do_reboot();
};

#endif // NETMGR_H
