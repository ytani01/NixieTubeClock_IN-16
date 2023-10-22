/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_WifiMgr.h"

/** static variables
 *
 */
ConfFile_Ssid *Task_WifiMgr::Obj_ConfFile_Ssid = NULL;
WiFiMulti *Task_WifiMgr::Obj_WiFiMulti = NULL;
WiFiEvent_t Task_WifiMgr::LastEvId = (WiFiEvent_t)NULL;
WiFiEventInfo_t Task_WifiMgr::LastEvInfo;

/**
 *
 */
Task_WifiMgr::Task_WifiMgr(std::string ap_ssid_hdr)
  : Task(Task_WifiMgr::NAME)
{
  this->ap_ssid_hdr = ap_ssid_hdr;

  WiFi.onEvent(Task_WifiMgr::on_wifi_event);

  Task_WifiMgr::Obj_ConfFile_Ssid = new ConfFile_Ssid();
  Task_WifiMgr::Obj_WiFiMulti = new WiFiMulti();
} // Task_WifiMgr::Task_WifiMgr


/** protected virtual
 *
 */
void Task_WifiMgr::setup() {
  log_d("%s", this->conf.name);

  int conf_n;
  if ( conf_n = Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    for (auto ent: Task_WifiMgr::Obj_ConfFile_Ssid->ent) {
      log_d("ent|%s|%s|", ent.first.c_str(), ent.second.c_str());
    }
  }

#if 0
  // Conf test
  
  //Task_WifiMgr::Obj_ConfFile_Ssid->remove();

  Task_WifiMgr::Obj_ConfFile_Ssid->ent["aaa"] = "AAA";
  Task_WifiMgr::Obj_ConfFile_Ssid->ent["bbb"] = "DDD";

  Task_WifiMgr::Obj_ConfFile_Ssid->ent.erase("aaa");
  conf_n = Task_WifiMgr::Obj_ConfFile_Ssid->save();
  log_d("conf_n = %d", conf_n);

  if ( conf_n = Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    for (auto ent: Task_WifiMgr::Obj_ConfFile_Ssid->ent) {
      log_d("%s:%s", ent.first.c_str(), ent.second.c_str());
    }
  }
#endif

  Task_WifiMgr::Obj_WiFiMulti->addAP("ytnet_a1x", "a1@ytnet");
  Task_WifiMgr::Obj_WiFiMulti->addAP("ytnet_ip", "ip@ytnet");

} // Task_WifiMgr::setup()

/** protected virtual
 *
 */
void Task_WifiMgr::loop() {
  if ( this->mode == WIFI_MGR_MODE_STA ) {
    this->loop_sta_mode();
    return;
  }

  if ( this->mode == WIFI_MGR_MODE_AP ) {
    this->loop_ap_mode();
    return;
  }
} // Task_WifiMgr::loop()

/**
 *
 */
void Task_WifiMgr::loop_sta_mode() {
  static bool prev_connected = false;
  static int retry_count = Task_WifiMgr::STA_RETRY_MAX;
  wl_status_t wl_stat;

  log_d("Task_WifiMgr::Obj_WiFiMulti->run()");

  if ( (wl_stat = (wl_status_t)Task_WifiMgr::Obj_WiFiMulti->run()) != WL_CONNECTED ) {
    log_e("%d/%d, WiFi not connected %d:%s (LastEvent:%d)",
          retry_count, Task_WifiMgr::STA_RETRY_MAX,
          wl_stat, WL_STATUS_T_STR[wl_stat],
          Task_WifiMgr::LastEvId);

    prev_connected = false;

    if ( --retry_count <= 0 ) {
      log_e("retry fail");
    }
  } else if ( ! prev_connected ) {
    log_i("WiFi connected (%d)", Task_WifiMgr::LastEvId);

    prev_connected = true;

    retry_count = Task_WifiMgr::STA_RETRY_MAX;
  }
  delay(1000);
} // Task_WifiMgr::loop_sta_mode() {

/**
 *
 */
void Task_WifiMgr::loop_ap_mode() {
  log_i("");
  delay(10000);
} // Task_WifiMgr::loop_ap_mode()

/**
 *
 */
void Task_WifiMgr::on_wifi_event(WiFiEvent_t ev_id, WiFiEventInfo_t ev_info) {
  log_d("ev_id = %d", ev_id);

  Task_WifiMgr::LastEvId = ev_id;
  Task_WifiMgr::LastEvInfo = ev_info;
  
  char buf[128];

  switch (ev_id) {
  case ARDUINO_EVENT_WIFI_READY: 
    sprintf(buf, "WiFi interface ready");
    break;
  case ARDUINO_EVENT_WIFI_SCAN_DONE:
    sprintf(buf, "Completed scan for access points");
    break;
  case ARDUINO_EVENT_WIFI_STA_START:
    sprintf(buf, "WiFi client started");
    break;
  case ARDUINO_EVENT_WIFI_STA_STOP:
    sprintf(buf, "WiFi clients stopped");
    break;
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    sprintf(buf, "Connected to access point");
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    sprintf(buf, "Disconnected from WiFi access point: %u",
            ev_info.wifi_sta_disconnected.reason);
    break;
  case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
    sprintf(buf, "Authentication mode of access point has changed");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    sprintf(buf, "Obtained IP address: %s", WiFi.localIP().toString().c_str());
    break;
  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    sprintf(buf, "Lost IP address and IP address is reset to 0");
    break;
  case ARDUINO_EVENT_WPS_ER_SUCCESS:
    sprintf(buf, "WiFi Protected Setup (WPS): succeeded in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_FAILED:
    sprintf(buf, "WiFi Protected Setup (WPS): failed in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_TIMEOUT:
    sprintf(buf, "WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_PIN:
    sprintf(buf, "WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  case ARDUINO_EVENT_WIFI_AP_START:
    sprintf(buf, "WiFi access point started");
    break;
  case ARDUINO_EVENT_WIFI_AP_STOP:
    sprintf(buf, "WiFi access point  stopped");
    break;
  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
    sprintf(buf, "Client connected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    sprintf(buf, "Client disconnected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
    sprintf(buf, "Assigned IP address to client");
    break;
  case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
    sprintf(buf, "Received probe request");
    break;
  case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
    sprintf(buf, "AP IPv6 is preferred");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    sprintf(buf, "STA IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP6:
    sprintf(buf, "Ethernet IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_START:
    sprintf(buf, "Ethernet started");
    break;
  case ARDUINO_EVENT_ETH_STOP:
    sprintf(buf, "Ethernet stopped");
    break;
  case ARDUINO_EVENT_ETH_CONNECTED:
    sprintf(buf, "Ethernet connected");
    break;
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    sprintf(buf, "Ethernet disconnected");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP:
    sprintf(buf, "Obtained IP address");
    break;
  default: break;
  } // switch

  log_i("%d:%s", ev_id, buf);
} // Task_WifiMgr::on_wifi_event()
