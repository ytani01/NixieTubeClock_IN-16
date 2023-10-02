/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_NetMgr.h"

/**
 *
 */
Task_NetMgr::Task_NetMgr(String name, String ap_ssid_hdr,
                       NetMgrInfo_t *netmgr_info,
                       unsigned long wifi_try_count):
  Task(String(this->name)) {

  this->ap_ssid_hdr = ap_ssid_hdr;
  this->wifi_try_count = wifi_try_count;
  this->netmgr_info = netmgr_info;
} // Task_NetMgr::Task_NetMgr

/**
 *
 */
void Task_NetMgr::restart_wifi() {
  log_i("");
  this->netMgr->restart();
} // Task_NetMgr::restart_wifi()

/**
 *
 */
void Task_NetMgr::clear_ssid() {
  log_i("");
  this->netMgr->save_ssid("", "");
} // Task_NetMgr::clear_ssid()

/**
 *
 */
void Task_NetMgr::setup() {
  log_i("%s", this->conf.name);

  this->netMgr = new NetMgr(this->ap_ssid_hdr, this->wifi_try_count);

  this->netmgr_info->ap_ssid = this->netMgr->ap_ssid;
  for (int i=0; i < 6; i++) {
    this->netmgr_info->mac_addr[i] = this->netMgr->mac_addr[i];
  } // for(i)
} // Task_NetMgr::setup()

/**
 *
 */
void Task_NetMgr::loop() {
  static NetMgrMode_t prev_mode = NETMGR_MODE_NULL;
  NetMgrMode_t mode = this->netMgr->loop();

  this->netmgr_info->mode = mode;
  this->netmgr_info->ssid = this->netMgr->cur_ssid;
  if ( mode == NETMGR_MODE_WIFI_ON ) {
      this->netmgr_info->ip_addr = this->netMgr->ip_addr;
      if ( prev_mode != mode ) {
        log_d("ip_addr:%s", this->netmgr_info->ip_addr.toString().c_str());
      }
  }

  prev_mode = mode;
} // Task_NetMgr::loop()
