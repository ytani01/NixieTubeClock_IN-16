/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "NtpTask.h"

/**
 * defulat callback
 */
static void _ntp_cb(NtpTaskInfo_t *ntp_info) {
  log_i("ntp_info.sntp_stat=%d", ntp_info->sntp_stat);
} // _ntp_cb()

/** static function
 *
 */
char* NtpTask::get_time_str() {
  struct tm ti; // time info
  const String day_str[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
  static char buf[4+1+2+1+2 +1+3+1 +1 +2+1+2+1+2 +1];

  getLocalTime(&ti);
  strftime(buf, sizeof(buf), "%Y-%m-%d(%a) %H:%M:%S", &ti);
  return buf;
} // NetMgrTask::get_time_str()

/** constructor
 *
 */
NtpTask::NtpTask(String ntp_svr[], NetMgrTask **pNetMgrTask,
                           void (*cb)(NtpTaskInfo_t *ntp_info))
  : Task("NTP_task") {

  this->ntp_svr = ntp_svr;
  this->pNetMgrTask = pNetMgrTask;
  this->_cb = cb;
  if ( cb == NULL ) {
    this->_cb = _ntp_cb;
  }

  this->info.sntp_stat = SNTP_SYNC_STATUS_RESET;
} // NtpTask::NtpTask

/**
 *
 */
void *NtpTask::get_info() {
  return (void *)&(this->info);
} // NtpTask::get_info()

/**
 *
 */
void NtpTask::setup() {
  log_d("%s", this->conf.name);

  setenv("TZ", "JST-9", 1);
  tzset();
  //sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
  sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
} // NtpTask::setup()

/**
 *
 */
void NtpTask::loop() {
  bool wifi_available = false;
  unsigned long interval = INTERVAL_NO_WIFI;

  NetMgrTask *netMgrTask = *(this->pNetMgrTask);
  if ( netMgrTask != NULL ) {
    NetMgr *netMgr = netMgrTask->netMgr;
    if ( netMgr != NULL ) {
      NetMgrMode_t netMgrMode = netMgr->cur_mode;
      if ( netMgrMode == NETMGR_MODE_WIFI_ON ) {
        wifi_available = true;
      }
    }
  }

  if ( ! wifi_available ) {
    log_w("WIFI is not available");
    this->info.sntp_stat = SNTP_SYNC_STATUS_RESET;

    this->_cb(&(this->info));
    delay(INTERVAL_NO_WIFI);
    return;
  }
    
  // start sync
  log_d("start sync ..");
  configTime(9 * 3600L, 0,
             ntp_svr[0].c_str(), ntp_svr[1].c_str(), ntp_svr[2].c_str());

  /*
   * sntp_get_sync_status()
   *   同期未完了の場合、SNTP_SYNC_STATUS_RESET
   *   動機が完了すると「一度だけ」、SNTP_SYNC_STATUS_COMPLETE
   *   SNTP_SYNC_MODE_SMOOTHの同期中の場合は、SNTP_SYNC_STAUS_IN_PROGRESS)
   */
  static sntp_sync_status_t prev_stat = SNTP_SYNC_STATUS_RESET;
  this->info.sntp_stat = sntp_get_sync_status();
  if ( this->info.sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
    interval = INTERVAL_NORMAL;
    if ( prev_stat != SNTP_SYNC_STATUS_COMPLETED ) {
      log_i("%s: NTP sync done: sntp_stat=%s(%d), interval=%'d",
            get_time_str(),
            SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
            interval);
    }

  } else if ( this->info.sntp_stat == SNTP_SYNC_STATUS_IN_PROGRESS ) {
    interval = INTERVAL_PROGRESS;
    log_i("%s: NTP sync progress: sntp_stat=%s(%d), interval=%'d",
          get_time_str(),
          SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
          interval);

  } else {
    interval = INTERVAL_NO_WIFI;
    log_i("%s: NTP sync retry: sntp_stat=%s(%d), interval=%'d",
          get_time_str(),
          SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
          interval);
  }

  this->_cb(&(this->info));

  prev_stat = this->info.sntp_stat;
  delay(interval);
} // NtpTask::loop()
