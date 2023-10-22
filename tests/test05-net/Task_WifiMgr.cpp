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

WebServer Task_WifiMgr::web_svr;

/**
 *
 */
Task_WifiMgr::Task_WifiMgr(std::string ap_ssid_hdr)
  : Task(Task_WifiMgr::NAME)
{
  this->ap_ssid = ap_ssid_hdr + get_mac_addr_string();
  log_i("ap_ssid = \"%s\"", this->ap_ssid.c_str());

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

  if ( retry_count <= 0 ) {
    //
    // failed
    //
    log_e("retry fail");

    retry_count = Task_WifiMgr::STA_RETRY_MAX;

    this->mode = WIFI_MGR_MODE_AP;
    return;
  }

  log_i("%d/%d Task_WifiMgr::Obj_WiFiMulti->run()",
        retry_count, Task_WifiMgr::STA_RETRY_MAX);

  this->wl_stat = (wl_status_t)Task_WifiMgr::Obj_WiFiMulti->run();

  if ( this->wl_stat == WL_CONNECTED ) {
    //
    // connected
    //
    if ( ! prev_connected ) {
      log_i("WiFi connected (%d)", Task_WifiMgr::LastEvId);

      prev_connected = true;

      retry_count = Task_WifiMgr::STA_RETRY_MAX;
    }
    delay(2000);
    return;
  }

  //
  // not connected
  //
  log_e("wl_stat = %d:%s (LastEvent:%d)",
        wl_stat, WL_STATUS_T_STR[wl_stat],
        Task_WifiMgr::LastEvId);

  prev_connected = false;

  --retry_count;

  delay(1000);
  return;
} // Task_WifiMgr::loop_sta_mode() {

/**
 *
 */
void Task_WifiMgr::loop_ap_mode() {
  if ( Task_WifiMgr::LastEvId != ARDUINO_EVENT_WIFI_AP_START ) {
    log_i("ap_ssid=%s", this->ap_ssid.c_str());

    if ( ! WiFi.softAP(this->ap_ssid.c_str()) ) {
      log_e("softAP(\"%s\") ... failed !?", this->ap_ssid);

      this->mode = WIFI_MGR_MODE_STA;
      delay(2000);
      return;
    }

    this->ap_ip = WiFi.softAPIP();
    this->ap_netmask = WiFi.softAPSubnetMask();
    log_i("my_ip: %s/%s",
          this->ap_ip.toString().c_str(),
          this->ap_netmask.toString().c_str());

    this->wl_stat = WiFi.status();
    log_i("wl_stat = %d:%s", this->wl_stat, WL_STATUS_T_STR[this->wl_stat]);

  }

  // ARDUINO_EVENT_WIFI_AP_START

  // DNS server
  this->dns_svr.setErrorReplyCode(DNSReplyCode::NoError);
  this->dns_svr.start(Task_WifiMgr::DNS_PORT, "*", this->ap_ip);
  log_i("DNS Server[%d] started", Task_WifiMgr::DNS_PORT);
  
  // Web server
  Task_WifiMgr::web_svr.enableDelay(false); // Importand !!

  Task_WifiMgr::web_svr.on("/", Task_WifiMgr::handle_top);
  Task_WifiMgr::web_svr.onNotFound(Task_WifiMgr::handle_top);

  Task_WifiMgr::web_svr.begin();
  log_i("Web Server[%d] started", Task_WifiMgr::WEB_PORT);

  // AP loop T.B.D.
  while ( true ) {
    this->dns_svr.processNextRequest();
    Task_WifiMgr::web_svr.handleClient();
    delay(100);
  }
  
  this->mode = WIFI_MGR_MODE_STA;
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

/** static
 *
 */
std::string Task_WifiMgr::html_header(std::string title) {
  std::string html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";

  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='Pragma' content='no-cache'>";
  html += "<meta http-equiv='Cache-Control' content='no-cache'>";
  html += "<meta http-equiv='Expires' content='0'>";
  html += "<meta http-equiv='Content-type' CONTENT='text/html; charset=utf-8'>";

  html += "<style>";
  html += ".myname {";
  html += "  font-size: xx-large;";
  html += "  text-align: center;";
  html += "  color: #FF0000;";
  html += "  background-color: #444444;";
  html += "}";

  html += ".ssid {";
  html += " font-size: x-large;";
  html += "}";

  html += "a:link, a:visited {";
  html += " background-color: #00AA00;";
  html += " color: white;";
  html += " border: none;";
  html += " text-align: center;";
  html += " text-decoration: none;";
  html += " display: inline-block;";
  html += " padding: 3px 7px;";
  html += "}";

  html += "a:hover, a:active {";
  html += " background-color: #00DD00;";
  html += "}";

  html += "input[type=button], input[type=submit], input[type=reset] {";
  html += " background-color: #0000DD;";
  html += " color: white;";
  html += " text-decoration: none;";
  html += " font-size: large;";
  html += " border: none;";
  html += " padding: 4px 8px;";
  html += "}";
  html += "</style>";

  html += "</head>";
  html += "<body style='background: linear-gradient(to right, #9FF, #5DD);'>";
  html += "<br />";
  html += "<div class='myname'>";
  html += "WifiMgr";
  html += "</div>";
  html += "<div style='font-size:x-large; color:#00F; background-color: #DDD;'>";
  html += title;
  html += "</div>";
  html += "<hr>";
  return html;
} // Task_WifiMgr::html_header()

/**
 *
 */
std::string Task_WifiMgr::html_footer() {
  std::string html = "";
  html += "</body>";
  html += "</html>\n";
  return html;
} // Task_WifiMgr::html_footer();

/**
 *
 */
void Task_WifiMgr::handle_top() {
  std::string ssid = "", pw = "";
  
  log_i("");

#if 0
  if ( Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    ssid = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->first.c_str();
    pw = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->second.c_str();
  }
  log_d("ssid=%s, pw=%s", ssid.c_str(), pw.c_str());
#endif

  std::string html = Task_WifiMgr::html_header("Current settings");

  html += "<span style='font-size: large;'>";
  html += "SSID: ";
  html += "</span>";
  html += "<span style='font-size: x-large; font-weight: bold'>";
  html += ssid;
  html += "</span>";
  html += "<hr />";
  html += "<a href='/select_ssid'>Change</a>\n";
  html += "or\n";
  html += "<a href='/confirm_reboot'>OK</a>\n";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", String(html.c_str()));
} // Task_WifiMgr::handle_top()

/**
 *
 */
void Task_WifiMgr::handle_select_ssid() {
  std::string ssid = "", pw = "";
  
  if ( Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    ssid = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->first.c_str();
    pw = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->second.c_str();
  }
  log_d("ssid=%s, pw=%s", ssid.c_str(), pw.c_str());
  
#if 0
  log_d("scan SSIDs ...");
  uint16_t ssid_n = WiFi.scanNetworks();
  log_d("ssid_n=%s", ssid_n);

  if (ssid_n <= 0) {
    log_d("rescan SSID..");
    uint16_t ssid_n = WiFi.scanNetworks();
    log_d("ssid_n = %s", ssid_n);
  }

  if ( ssid_n > Task_WifiMgr::SSID_N_MAX ) {
    ssid_n = Task_WifiMgr::SSID_N_MAX;
    log_d("ssid_n = %d", ssid_n);
  }
  if ( ssid_n < 0 ) {
    ssid_n = 0;
    log_d("ssid_n = %d", ssid_n);
  }

  SSIDent ssid_ent[Task_WifiMgr::SSID_N_MAX];

  for (int i=0; i < ssid_n; i++) {
    ssid_ent[i].set(WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i));
    log_d("  %s", ssid_ent[i].toString().c_str());
  }
  WiFi.scanDelete();
#endif
  
  std::string html = Task_WifiMgr::html_header("Please change settings and save");

  html += "<form action='/save_ssid' method='GET'>";
  html += "<div class='ssid'>";
  html += "SSID ";
  html += "<select name='ssid' id='ssid' style='font-size:large;'>";

#if 0
  for(int i=0; i < ssid_n; i++){
    html += "<option value=" + ssid_ent[i].ssid();
    if ( ssid_ent[i].ssid() == ssid ) {
      html += " selected";
    }
    html += ">";
    html += ssid_ent[i].ssid();
    /*
    html += " (";
    html += String(ssid_ent[i].dbm());
    html += ", ";
    html += ssid_ent[i].encType();
    html += ")";
    */
    html += "</option>\n";
  } // for(i)
#endif
  
  html += "<option value="">(clear)</option>\n";
  html += "</select><br />\n";

  html += "Password ";
  html += "<span style='font-size: xx-large'>";
  html += "<input type='text'";
  html += " name='passwd'";
  html += " value='" + pw + "'";
  html += " />";
  html += "</span>";
  html += "</div>\n";
  html += "<hr />\n";

  html += "<input type='submit' value='Save' />\n";
  html += "<a href='/scan_ssid'>Rescan</a>\n";
  html += "<a href='/'>Cancel</a>\n";

  html += "</form>";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());
} // Task_WifiMgr::handle_select_ssid()

/**
 *
 */
void Task_WifiMgr::handle_save_ssid(){
  std::string ssid = Task_WifiMgr::web_svr.arg("ssid").c_str();
  std::string pw = Task_WifiMgr::web_svr.arg("passwd").c_str();
  
  log_d("save_ssid> |%s|%s|", ssid.c_str(), pw.c_str());

  Task_WifiMgr::Obj_ConfFile_Ssid->ent[ssid.c_str()] = pw.c_str();
  Task_WifiMgr::Obj_ConfFile_Ssid->save();

  // 自動転送
  Task_WifiMgr::web_svr.sendHeader("Location", String("/"), true);
  Task_WifiMgr::web_svr.send(302, "text/plain", "");
} // Task_WifiMgr::handle_save_ssid()

/**
 *
 */
void Task_WifiMgr::handle_confirm_reboot() {
  std::string html = Task_WifiMgr::html_header("Reboot confirmation");

  html += "<p>Are you sure to reboot ";
  html += "WiFiMgr";
  html += " ?</p>\n";
  html += "<a href='/do_reboot'>Yes</a>";
  html += " or ";
  html += "<a href='/'>No</a>";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());
} // Task_WifiMgr::handle_confirm_reboot()

/**
 *
 */
void Task_WifiMgr::handle_do_scan() {
  // 自動転送
  Task_WifiMgr::web_svr.sendHeader("Location", String("/select_ssid"), true);
  Task_WifiMgr::web_svr.send(302, "text/plain", "");
} // Task_WifiMgr::handle_do_rescan()

/**
 *
 */
void Task_WifiMgr::handle_do_reboot() {
  std::string html = Task_WifiMgr::html_header("Rebooting ..");

  html += "Please reconnect WiFi ..";
  html += "<hr />";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());

  log_w("reboot esp32 ..");
  delay(2000);
  ESP.restart();
} // Task_WifiMgr::handle_do_reboot()
