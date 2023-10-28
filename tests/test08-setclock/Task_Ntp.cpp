/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_Ntp.h"

/**
 * defulat callback
 */
static void _ntp_cb(Task_NtpInfo_t *ntp_info) {
  log_d("ntp_info.sntp_stat=%d:%s",
        ntp_info->sntp_stat, SNTP_SYNC_STATUS_STR[ntp_info->sntp_stat]);
} // _ntp_cb()

/** constructor
 *
 */
Task_Ntp::Task_Ntp(String ntp_svr[],
                   void (*cb)(Task_NtpInfo_t *ntp_info))
  : Task("NTP_task") {

  this->ntp_svr = ntp_svr;
  this->_cb = cb;
  if ( cb == NULL ) {
    this->_cb = _ntp_cb;
  }

  this->info.sntp_stat = SNTP_SYNC_STATUS_RESET;
} // Task_Ntp::Task_Ntp

/**
 *
 */
void *Task_Ntp::get_info() {
  return (void *)&(this->info);
} // Task_Ntp::get_info()

/**
 *
 */
void Task_Ntp::setup() {
  log_d("%s", this->conf.name.c_str());

  sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
} // Task_Ntp::setup()

/**
 *
 */
void Task_Ntp::loop() {
  static sntp_sync_status_t prev_stat = SNTP_SYNC_STATUS_RESET;

  unsigned long interval = INTERVAL_NO_WIFI;

  //
  // get WiFi status
  //
  bool wifi_available = false;

  wifi_mgr_mode_t wifimgr_mode = TaskWifiMgr->mode;
  wl_status_t wl_stat = TaskWifiMgr->wl_stat;
  
  if ( wifimgr_mode == WIFI_MGR_MODE_STA ) {
    if ( wl_stat == WL_CONNECTED ) {
      wifi_available = true;
    }
  }

  if ( ! wifi_available ) {
    //
    // WiFi is not available
    //
    log_d("WIFI is not available");
    this->info.sntp_stat = SNTP_SYNC_STATUS_RESET;

    this->_cb(&(this->info));
    delay(INTERVAL_NO_WIFI);
    return;
  }

  //
  // start NTP sync
  //
  log_d("start sync: %s ..", SNTP_SYNC_STATUS_STR[prev_stat]);

  configTime(9 * 3600L, 0,
             ntp_svr[0].c_str(), ntp_svr[1].c_str(), ntp_svr[2].c_str());

  /*
  configTzTime("JST-9",
               ntp_svr[0].c_str(), ntp_svr[1].c_str(), ntp_svr[2].c_str());
  */

  /*
   * sntp_get_sync_status()
   *   同期未完了の場合、SNTP_SYNC_STATUS_RESET
   *   同期が完了すると「一度だけ」、SNTP_SYNC_STATUS_COMPLETE
   *   SNTP_SYNC_MODE_SMOOTHの同期中の場合は、SNTP_SYNC_STAUS_IN_PROGRESS)
   */
  this->info.sntp_stat = sntp_get_sync_status();
  log_d("sntp_stat = %s ..", SNTP_SYNC_STATUS_STR[this->info.sntp_stat]);

  if ( this->info.sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
    interval = INTERVAL_NORMAL;    
    if ( prev_stat != SNTP_SYNC_STATUS_COMPLETED ) {
      log_d("%s: NTP sync done: sntp_stat=%s(%d), interval=%'d",
            SysClock::now_string().c_str(),
            SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
            interval);
    }

  } else if ( this->info.sntp_stat == SNTP_SYNC_STATUS_IN_PROGRESS ) {
    interval = INTERVAL_PROGRESS;
    log_d("%s: NTP sync progress: sntp_stat=%s(%d), interval=%'d",
          SysClock::now_string().c_str(),
          SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
          interval);

  } else {
    interval = INTERVAL_NO_WIFI;
    log_d("%s: NTP sync retry: sntp_stat=%s(%d), interval=%'d",
          SysClock::now_string().c_str(),
          SNTP_SYNC_STATUS_STR[this->info.sntp_stat], this->info.sntp_stat,
          interval);
  }

  this->_cb(&(this->info));

  prev_stat = this->info.sntp_stat;
  delay(interval);
} // Task_Ntp::loop()
