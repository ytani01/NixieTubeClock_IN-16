/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _NTP_TASK_H_
#define _NTP_TASK_H_

#include <esp_sntp.h>
#include "Task.h"
#include "NetMgrTask.h"

static const char *SNTP_SYNC_STATUS_STR[] = {"RESET", "COMPLETED", "IN_PROGRESS"};

typedef struct {
  sntp_sync_status_t sntp_stat;
} NtpTaskInfo_t;

/**
 *
 */
class NtpTask: public Task {
public:
  const unsigned long INTERVAL_NORMAL = 1 * 60 * 1000; // ms
  const unsigned long INTERVAL_PROGRESS = 10 * 1000; // ms
  const unsigned long INTERVAL_NO_WIFI = 5 * 1000; // ms
  
  String *ntp_svr;
  NetMgrTask **pNetMgrTask = NULL;

  NtpTaskInfo_t info;
  
  // static function
  static char* get_time_str();

  // constructor
  NtpTask(String ntp_svr[], NetMgrTask **pNetMgrTask,
          void (*cb)(NtpTaskInfo_t *ntp_info)=NULL);

  void *get_info();

protected:
  virtual void setup();
  virtual void loop();

  void (*_cb)(NtpTaskInfo_t *ntp_info);
}; // class NtpTask
#endif // _NTP_TASK_H_
