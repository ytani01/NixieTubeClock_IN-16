/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _TASK_NTP_H_
#define _TASK_NTP_H_

#include <esp_sntp.h>
#include "Task.h"
#include "SysClock.h"
#include "MyRtc.h"
#include "Task_WifiMgr.h"

extern MyRtc *Rtc;
extern Task_WifiMgr *TaskWifiMgr;

static const char *SNTP_SYNC_STATUS_STR[] = {"RESET", "COMPLETED", "IN_PROGRESS"};

typedef struct {
  sntp_sync_status_t sntp_stat;
} Task_NtpInfo_t;

/**
 *
 */
class Task_Ntp: public Task {
public:
  static const uint32_t STACK_SIZE = 4 * 1024;
  static const UBaseType_t PRIORITY = 0;
  static const UBaseType_t CORE = APP_CPU_NUM;

  static const unsigned long INTERVAL_NORMAL = 1 * 60 * 1000; // ms
  static const unsigned long INTERVAL_PROGRESS = 10 * 1000; // ms
  static const unsigned long INTERVAL_NO_WIFI = 10 * 1000; // ms
  
  String *ntp_svr;

  Task_NtpInfo_t info;
  sntp_sync_status_t prev_stat = SNTP_SYNC_STATUS_RESET;
  
  // constructor
  Task_Ntp(String ntp_svr[],
           void (*cb)(Task_NtpInfo_t *ntp_info)=NULL,
           uint32_t stack_size=STACK_SIZE,
           UBaseType_t priority=PRIORITY,
           UBaseType_t core=CORE);

  void start_sync();

  Task_NtpInfo_t *get_info();

protected:
  virtual void setup();
  virtual void loop();

  void (*_cb)(Task_NtpInfo_t *ntp_info);
}; // class Task_Ntp
#endif // _TASK_NTP_H_
