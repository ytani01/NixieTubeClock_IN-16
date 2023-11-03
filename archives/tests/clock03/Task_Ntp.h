/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_NTP_H_
#define _TASK_NTP_H_

#include <esp_sntp.h>
#include "Task.h"
#include "Task_NetMgr.h"

static const char *SNTP_SYNC_STATUS_STR[] = {"RESET", "COMPLETED", "IN_PROGRESS"};

typedef struct {
  sntp_sync_status_t sntp_stat;
} Task_NtpInfo_t;

/**
 *
 */
class Task_Ntp: public Task {
public:
  const unsigned long INTERVAL_NORMAL = 1 * 60 * 1000; // ms
  const unsigned long INTERVAL_PROGRESS = 10 * 1000; // ms
  const unsigned long INTERVAL_NO_WIFI = 5 * 1000; // ms
  
  String *ntp_svr;
  Task_NetMgr **pTaskNetMgr = NULL;

  Task_NtpInfo_t info;
  
  // static function
  static char* get_time_str();

  // constructor
  Task_Ntp(String ntp_svr[], Task_NetMgr **pTaskNetMgr,
           void (*cb)(Task_NtpInfo_t *ntp_info)=NULL);

  void *get_info();

protected:
  virtual void setup();
  virtual void loop();

  void (*_cb)(Task_NtpInfo_t *ntp_info);
}; // class Task_Ntp
#endif // _TASK_NTP_H_
