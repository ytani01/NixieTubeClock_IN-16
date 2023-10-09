/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_NETMGR_H_
#define _TASK_NETMGR_H_

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
class Task_NetMgr: public Task {
 public:
  NetMgr *netMgr = NULL;
  String ap_ssid_hdr;
  unsigned long wifi_try_count;

  static constexpr char *name = (char *)"Task:NetMgr";
  
  Task_NetMgr(String name, String ap_ssid_hdr, NetMgrInfo_t *netmgr_info);

  void set_mode(NetMgrMode_t mode);
  void restart_wifi();
  void clear_ssid();

 protected:
  NetMgrInfo_t *netmgr_info;

  virtual void setup();
  virtual void loop();

}; // class Task_NetMgr
#endif // _TASK_NETMGR_H_
