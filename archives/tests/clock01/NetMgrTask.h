/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _NETMGR_TASK_H_
#define _NETMGR_TASK_H_

#include "Task.h"
#include "NetMgr.h"

typedef struct {
  NetMgrMode_t mode;
  uint8_t mac_addr[6];
  IPAddress ip_addr;
  String new_ssid;
  String ssid;
  String ap_ssid;
} NetMgrInfo_t;

/**
 *
 */
class NetMgrTask: public Task {
 public:
  NetMgr *netMgr = NULL;
  String ap_ssid_hdr;
  unsigned long wifi_try_count;

  NetMgrTask(String name, String ap_ssid_hdr,
                  NetMgrInfo_t *netmgr_info,
                  unsigned long wifi_try_count=NetMgr::DEF_TRY_COUNT_MAX);

  void restart_wifi();
  void clear_ssid();

 protected:
  NetMgrInfo_t *netmgr_info;

  virtual void setup();
  virtual void loop();

}; // class NetMgrTask
#endif // _NETMGR_TASK_H_
