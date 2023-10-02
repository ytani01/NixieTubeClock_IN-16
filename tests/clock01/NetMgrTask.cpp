/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "NetMgrTask.h"

/**
 *
 */
NetMgrTask::NetMgrTask(String name, String ap_ssid_hdr,
                       NetMgrInfo_t *netmgr_info,
                       unsigned long wifi_try_count):
  Task(name + "_task") {

  this->ap_ssid_hdr = ap_ssid_hdr;
  this->wifi_try_count = wifi_try_count;
  this->netmgr_info = netmgr_info;
} // NetMgrTask::NetMgrTask

/**
 *
 */
void NetMgrTask::restart_wifi() {
  log_i("");
  this->netMgr->restart();
} // NetMgrTask::restart_wifi()

/**
 *
 */
void NetMgrTask::clear_ssid() {
  log_i("");
  this->netMgr->save_ssid("", "");
} // NetMgrTask::clear_ssid()

/**
 *
 */
void NetMgrTask::setup() {
  log_d("%s", this->conf.name);
  this->netMgr = new NetMgr(this->ap_ssid_hdr, this->wifi_try_count);

  this->netmgr_info->ap_ssid = this->netMgr->ap_ssid;
  for (int i=0; i < 6; i++) {
    this->netmgr_info->mac_addr[i] = this->netMgr->mac_addr[i];
  } // for(i)
} // NetMgrTask::setup()

/**
 *
 */
void NetMgrTask::loop() {
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
} // NetMgrTask::loop()
