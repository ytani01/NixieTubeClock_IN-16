/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Task_WifiMgr.h"
#include "Mode.h"

/** static variables
 *
 */
ConfFile_Ssid *Task_WifiMgr::Obj_ConfFile_Ssid = NULL;
WiFiMulti *Task_WifiMgr::Obj_WiFiMulti = NULL;

WiFiEvent_t Task_WifiMgr::LastEvId = (WiFiEvent_t)NULL;
char Task_WifiMgr::LastEvStr[128];
WiFiEventInfo_t Task_WifiMgr::LastEvInfo;

WebServer Task_WifiMgr::web_svr;

/**
 *
 */
Task_WifiMgr::Task_WifiMgr(std::string ap_ssid_hdr,
                           uint32_t stack_size,
                           UBaseType_t priority,
                           UBaseType_t core)
  : Task(__CLASS_NAME__, stack_size, priority, core)
{
  this->ap_ssid = ap_ssid_hdr + get_mac_addr_string();
  log_i("[WifiMgr] %s: ap_ssid = \"%s\"",
        this->conf.name.c_str(), this->ap_ssid.c_str());

  WiFi.onEvent(Task_WifiMgr::on_wifi_event);

  Task_WifiMgr::Obj_ConfFile_Ssid = new ConfFile_Ssid();

  log_i("uxTaskGetStackHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
} // Task_WifiMgr::Task_WifiMgr

/** static
 *
 */
void Task_WifiMgr::on_wifi_event(WiFiEvent_t ev_id, WiFiEventInfo_t ev_info) {
  //log_d("[WifiMgr] ev_id = %d", ev_id);

  Task_WifiMgr::LastEvId = ev_id;
  Task_WifiMgr::LastEvInfo = ev_info;
  
  switch (ev_id) {
  case ARDUINO_EVENT_WIFI_READY: 
    //sprintf(Task_WifiMgr::LastEvStr, "WiFi interface ready");
    sprintf(Task_WifiMgr::LastEvStr, "READY");
    break;
  case ARDUINO_EVENT_WIFI_SCAN_DONE:
    //sprintf(Task_WifiMgr::LastEvStr, "Completed scan for access points");
    sprintf(Task_WifiMgr::LastEvStr, "SCAN DONE");
    break;
  case ARDUINO_EVENT_WIFI_STA_START:
    //sprintf(Task_WifiMgr::LastEvStr, "WiFi client started");
    sprintf(Task_WifiMgr::LastEvStr, "STA START");
    break;
  case ARDUINO_EVENT_WIFI_STA_STOP:
    //sprintf(Task_WifiMgr::LastEvStr, "WiFi clients stopped");
    sprintf(Task_WifiMgr::LastEvStr, "STA STOP");
    break;
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    //sprintf(Task_WifiMgr::LastEvStr, "Connected to access point");
    sprintf(Task_WifiMgr::LastEvStr, "CONNECTED");
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    //sprintf(Task_WifiMgr::LastEvStr,
    //        "Disconnected from WiFi access point: %u",
    //        ev_info.wifi_sta_disconnected.reason);
    sprintf(Task_WifiMgr::LastEvStr, "DISCONNECTED:%u",
            ev_info.wifi_sta_disconnected.reason);
    break;
  case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
    //sprintf(Task_WifiMgr::LastEvStr,
    //        "Authentication mode of access point has changed");
    sprintf(Task_WifiMgr::LastEvStr, "AUTHMODE_CHANGE");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    //sprintf(Task_WifiMgr::LastEvStr,
    //        "Obtained IP address: %s", WiFi.localIP().toString().c_str());
    sprintf(Task_WifiMgr::LastEvStr, "GOT_IP:%s",
            WiFi.localIP().toString().c_str());
    break;
  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    //sprintf(Task_WifiMgr::LastEvStr,
    //        "Lost IP address and IP address is reset to 0");
    sprintf(Task_WifiMgr::LastEvStr, "LOST_IP");
    break;
  case ARDUINO_EVENT_WPS_ER_SUCCESS:
    //sprintf(Task_WifiMgr::LastEvStr,
    //        "WiFi Protected Setup (WPS): succeeded in enrollee mode");
    sprintf(Task_WifiMgr::LastEvStr, "WPS_ER_SUCCESS");
    break;
  case ARDUINO_EVENT_WPS_ER_FAILED:
    sprintf(Task_WifiMgr::LastEvStr,
            "WiFi Protected Setup (WPS): failed in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_TIMEOUT:
    sprintf(Task_WifiMgr::LastEvStr,
            "WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case ARDUINO_EVENT_WPS_ER_PIN:
    sprintf(Task_WifiMgr::LastEvStr,
            "WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  case ARDUINO_EVENT_WIFI_AP_START:
    //sprintf(Task_WifiMgr::LastEvStr, "WiFi access point started");
    sprintf(Task_WifiMgr::LastEvStr, "AP_START");
    break;
  case ARDUINO_EVENT_WIFI_AP_STOP:
    //sprintf(Task_WifiMgr::LastEvStr, "WiFi access point  stopped");
    sprintf(Task_WifiMgr::LastEvStr, "AP_STOP");
    break;
  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
    sprintf(Task_WifiMgr::LastEvStr, "Client connected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
    sprintf(Task_WifiMgr::LastEvStr, "Client disconnected");
    break;
  case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
    sprintf(Task_WifiMgr::LastEvStr, "Assigned IP address to client");
    break;
  case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
    sprintf(Task_WifiMgr::LastEvStr, "Received probe request");
    break;
  case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
    sprintf(Task_WifiMgr::LastEvStr, "AP IPv6 is preferred");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    sprintf(Task_WifiMgr::LastEvStr, "STA IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP6:
    sprintf(Task_WifiMgr::LastEvStr, "Ethernet IPv6 is preferred");
    break;
  case ARDUINO_EVENT_ETH_START:
    sprintf(Task_WifiMgr::LastEvStr, "Ethernet started");
    break;
  case ARDUINO_EVENT_ETH_STOP:
    sprintf(Task_WifiMgr::LastEvStr, "Ethernet stopped");
    break;
  case ARDUINO_EVENT_ETH_CONNECTED:
    sprintf(Task_WifiMgr::LastEvStr, "Ethernet connected");
    break;
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    sprintf(Task_WifiMgr::LastEvStr, "Ethernet disconnected");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP:
    sprintf(Task_WifiMgr::LastEvStr, "Obtained IP address");
    break;
  default:
    sprintf(Task_WifiMgr::LastEvStr, "(Unknown Event)");
    break;
  } // switch

  log_i("[WifiMgr] %d:%s", ev_id, Task_WifiMgr::LastEvStr);
} // Task_WifiMgr::on_wifi_event()

/** protected virtual
 *
 */
void Task_WifiMgr::setup() {
  log_d("[WifiMgr] %s", this->conf.name.c_str());
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

  log_i("[WifiMgr:%s]", WIFI_MGR_MODE_T_STR[this->mode]);

  //
  // Init STA
  //
  this->cur_ssid = "";
  
  Task_WifiMgr::Obj_WiFiMulti = new WiFiMulti();

  // add saved SSIDs
  int conf_n;
  if ( conf_n = Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    for (auto ent: Task_WifiMgr::Obj_ConfFile_Ssid->ent) {
      log_i("[WifiMgr] ent|%s|%s|", ent.first.c_str(), ent.second.c_str());

      Task_WifiMgr::Obj_WiFiMulti->addAP(ent.first.c_str(),
                                         ent.second.c_str());
    } // for
  } // if

  log_i("[WifiMgr:%s] conf_n = %d",
        WIFI_MGR_MODE_T_STR[this->mode], conf_n);

  if ( conf_n <= 0 ) {
    this->mode = WIFI_MGR_MODE_AP;
    return;
  }

  //
  // try to connect
  //
  for (int count=1; count <= Task_WifiMgr::STA_RETRY_MAX; count++) {
    if ( this->mode != WIFI_MGR_MODE_STA ) {
      break;
    }

    this->wl_stat = WiFi.status();
    
    log_i("[WifiMgr:%s] %d/%d Task_WifiMgr::Obj_WiFiMulti->run() <-- %d:%s",
          WIFI_MGR_MODE_T_STR[this->mode],
          count, Task_WifiMgr::STA_RETRY_MAX,
          this->wl_stat, WL_STATUS_T_STR2(this->wl_stat));

    // try to connect WiFi
    this->wl_stat = (wl_status_t)Task_WifiMgr::Obj_WiFiMulti->run();

    log_i("[WifiMgr:%s] %d/%d Task_WifiMgr::Obj_WiFiMulti->run() --> %d:%s",
          WIFI_MGR_MODE_T_STR[this->mode],
          count, Task_WifiMgr::STA_RETRY_MAX,
          this->wl_stat, WL_STATUS_T_STR2(this->wl_stat));

    if ( this->wl_stat == WL_CONNECTED ) {
      break;
    }

    if ( count >= Task_WifiMgr::STA_RETRY_MAX ) {
      break;
    }

    if ( this->mode != WIFI_MGR_MODE_STA ) {
      break;
    }

    if ( Task_WifiMgr::LastEvId == ARDUINO_EVENT_WIFI_STA_DISCONNECTED ) {
      if ( Task_WifiMgr::LastEvInfo.wifi_sta_disconnected.reason != 15 ) {
        log_i("delay(7000)");
        delay(7000);
      }
    }
    log_i("delay(5000)");
    delay(5000);
  } // for (count)

  delete Task_WifiMgr::Obj_WiFiMulti;

  if ( this->wl_stat != WL_CONNECTED ) {
    // not connected
    this->mode = WIFI_MGR_MODE_AP;
    return;
  }
  
  //
  // connected
  //
  this->cur_ssid = WiFi.SSID().c_str();

  log_d("[WifiMgr:%s] WiFi connected (%d:%s)",
        WIFI_MGR_MODE_T_STR[this->mode],
        Task_WifiMgr::LastEvId, Task_WifiMgr::LastEvStr);

  while ( (this->wl_stat = WiFi.status()) == WL_CONNECTED ) {
    log_v("[WifiMgr:%s] wl_stat = %d:%s, last event = %d:%s",
          WIFI_MGR_MODE_T_STR[this->mode],
          this->wl_stat, WL_STATUS_T_STR2(this->wl_stat),
          Task_WifiMgr::LastEvId, Task_WifiMgr::LastEvStr);

    delay(3000);
  }

  // disconnected !?
  this->cur_ssid = "";

  log_w("[WifiMgr:%s] wl_stat = %d:%s, last event = %d:%s",
        WIFI_MGR_MODE_T_STR[this->mode],
        this->wl_stat, WL_STATUS_T_STR2(this->wl_stat),
        Task_WifiMgr::LastEvId, Task_WifiMgr::LastEvStr);

  return; // retry
} // Task_WifiMgr::loop_sta_mode() {

/**
 *
 */
void Task_WifiMgr::loop_ap_mode() {
  WiFi.disconnect();
  delay(100);

  if ( Task_WifiMgr::LastEvId != ARDUINO_EVENT_WIFI_AP_START ) {
    log_i("[WifiMgr:AP] ap_ssid=%s", this->ap_ssid.c_str());

    if ( ! WiFi.softAP(this->ap_ssid.c_str()) ) {
      log_e("[WifiMgr:AP] softAP(\"%s\") ... failed !?", this->ap_ssid);

      this->mode = WIFI_MGR_MODE_STA;
      delay(2000);
      return;
    }
    delay(200);

    this->ap_ip = WiFi.softAPIP();
    this->ap_netmask = WiFi.softAPSubnetMask();
    log_i("[WifiMgr:AP] my_ip: %s/%s",
          this->ap_ip.toString().c_str(),
          this->ap_netmask.toString().c_str());

    this->wl_stat = WiFi.status();
    log_i("[WifiMgr:AP] wl_stat = %d:%s",
          this->wl_stat, WL_STATUS_T_STR2(this->wl_stat));

  }

  // ARDUINO_EVENT_WIFI_AP_START

  // DNS server
  this->dns_svr.setErrorReplyCode(DNSReplyCode::NoError);
  this->dns_svr.start(Task_WifiMgr::DNS_PORT, "*", this->ap_ip);
  log_i("[WifiMgr:AP] DNS Server[%d] started", Task_WifiMgr::DNS_PORT);
  
  // Web server
  Task_WifiMgr::web_svr.enableDelay(false); // Importand !!

  Task_WifiMgr::web_svr.on("/", Task_WifiMgr::handle_top);
  Task_WifiMgr::web_svr.on("/select_ssid", Task_WifiMgr::handle_select_ssid);
  Task_WifiMgr::web_svr.on("/save_ssid", Task_WifiMgr::handle_save_ssid);
  Task_WifiMgr::web_svr.on("/scan_ssid", Task_WifiMgr::handle_do_scan);
  Task_WifiMgr::web_svr.on("/confirm_reboot", Task_WifiMgr::handle_confirm_reboot);
  Task_WifiMgr::web_svr.on("/do_reboot", Task_WifiMgr::handle_do_reboot);

  //Task_WifiMgr::web_svr.onNotFound(Task_WifiMgr::handle_not_found);
  Task_WifiMgr::web_svr.onNotFound(Task_WifiMgr::handle_top);

  Task_WifiMgr::web_svr.begin();
  log_i("[WifiMgr:AP] Web Server[%d] started", Task_WifiMgr::WEB_PORT);

  // AP loop T.B.D.
  while ( true ) {
    this->dns_svr.processNextRequest();
    Task_WifiMgr::web_svr.handleClient();
    delay(10);
  }
  
  this->mode = WIFI_MGR_MODE_STA;
} // Task_WifiMgr::loop_ap_mode()

/** static
 *
 */
void Task_WifiMgr::handle_not_found() {
  log_i("[WifiMgr:AP] method = %d", Task_WifiMgr::web_svr.method());
  //Task_WifiMgr::web_svr.send(404, "text/plain", "NotFound");
  Task_WifiMgr::web_svr.send(200, "text/html; charset=utf-8", "Not Found");
} // Task_WifiMgr::handle_not_found()
  
/** static
 *
 */
void Task_WifiMgr::handle_top() {
  std::string ssid = "", pw = "";
  
  if ( Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    ssid = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->first.c_str();
    pw = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->second.c_str();
  }
  log_i("[WiFiMgr:AP] ssid=%s, pw=%s", ssid.c_str(), pw.c_str());

  std::string html = Task_WifiMgr::html_header("Current settings");

  html += "<span style='font-size: large;'>";
  html += "SSID: ";
  html += "</span>";
  html += "<span style='font-size: x-large; font-weight: bold'>";
  if ( ssid.size() == 0 ) {
    html += "(none)";
  } else {
    html += ssid;
  }
  html += "</span>";
  html += "<br />";
  html += "<span style='font-size: large;'>";
  html += "Passphrase: ";
  html += "</span>";
  html += "<span style='font-size: large; font-weight: bold'>";
  if ( pw.size() == 0 ) {
    html += "(none)";
  } else {
    html += pw[0];
    for (int i=0; i < pw.size() - 1; i++) {
      html += "*";
    }
  }
  html += "</span>";
  html += "<hr />";
  html += "<a href='/select_ssid'>Change</a>\n";
  html += "or\n";
  html += "<a href='/do_reboot'>OK (Reboot)</a>\n";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html; charset=utf-8", String(html.c_str()));
} // Task_WifiMgr::handle_top()

/** static
 *
 */
void Task_WifiMgr::handle_select_ssid() {
  std::string ssid = "", pw = "";
  
  WiFi.disconnect();

  if ( Task_WifiMgr::Obj_ConfFile_Ssid->load() > 0 ) {
    ssid = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->first.c_str();
    //pw = Task_WifiMgr::Obj_ConfFile_Ssid->ent.begin()->second.c_str();
  }
  log_i("[WifiMgr:AP] ssid=%s, pw=%s", ssid.c_str(), pw.c_str());
  
  log_i("[WifiMgr:AP] scan SSIDs ...");
  int16_t ssid_n = WiFi.scanNetworks();
  log_i("[WifiMgr:AP] ssid_n=%d", ssid_n);

  if ( ssid_n < 0 ) {
    ssid_n = 0;
    log_e("[WifiMgr:AP] ssid_n = %d", ssid_n);
  }

  std::string html = Task_WifiMgr::html_header("Please change settings and save");

  html += "<form action='/save_ssid' method='GET'>";
  html += "<div class='ssid'>";
  html += "SSID ";
  html += "<select name='ssid' id='ssid' style='font-size:large;'>";

  html += "<option value="">(clear)</option>\n";

  for(int i=0; i < ssid_n; i++){
    html += "<option value=" + std::string(WiFi.SSID(i).c_str());
    for ( auto ent: Task_WifiMgr::Obj_ConfFile_Ssid->ent ) {
      if ( std::string(WiFi.SSID(i).c_str()) == ent.first ) {
        html += " selected";
        //pw = Task_WifiMgr::Obj_ConfFile_Ssid->ent[ent.first];
        pw = ent.second;
      }
    }
    html += ">";
    html += std::string(WiFi.SSID(i).c_str());
    html += "</option>\n";
  } // for(i)

  html += "</select><br />\n";

  html += "Passphrase ";
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
  
  WiFi.scanDelete();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());
} // Task_WifiMgr::handle_select_ssid()

/** static
 *
 */
void Task_WifiMgr::handle_save_ssid() {
  std::string ssid = Task_WifiMgr::web_svr.arg("ssid").c_str();
  std::string pw = Task_WifiMgr::web_svr.arg("passwd").c_str();
  
  log_i("[WifiMgr:AP] |%s|%s|", ssid.c_str(), pw.c_str());

  Task_WifiMgr::Obj_ConfFile_Ssid->ent.clear(); // T.B.D.

  Task_WifiMgr::Obj_ConfFile_Ssid->ent[ssid.c_str()] = pw.c_str();
  Task_WifiMgr::Obj_ConfFile_Ssid->save();

  // 自動転送
  Task_WifiMgr::web_svr.sendHeader("Location", String("/"), true);
  Task_WifiMgr::web_svr.send(302, "text/plain", "");
} // Task_WifiMgr::handle_save_ssid()

/** static
 *
 */
void Task_WifiMgr::handle_confirm_reboot() {
  log_i("[WifiMgr:AP]");

  std::string html = Task_WifiMgr::html_header("Reboot confirmation");

  html += "<p>Are you sure to reboot ";
  html += "Nixie Clock";
  html += " ?</p>\n";
  html += "<a href='/do_reboot'>Yes</a>";
  html += " or ";
  html += "<a href='/'>No</a>";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());
} // Task_WifiMgr::handle_confirm_reboot()

/** static
 *
 */
void Task_WifiMgr::handle_do_scan() {
  log_i("[WifiMgr:AP]");

  // 自動転送
  Task_WifiMgr::web_svr.sendHeader("Location", String("/select_ssid"), true);
  Task_WifiMgr::web_svr.send(302, "text/plain", "");
} // Task_WifiMgr::handle_do_rescan()

/** static
 *
 */
void Task_WifiMgr::handle_do_reboot() {
  log_i("[WifiMgr:AP]");

  std::string html = Task_WifiMgr::html_header("Rebooting ..");

  html += "Please reconnect WiFi ..";
  html += "<hr />";
  html += Task_WifiMgr::html_footer();

  Task_WifiMgr::web_svr.send(200, "text/html", html.c_str());

  log_w("[WifiMgr:AP] reboot esp32 ..");
  //delay(2000);
  //ESP.restart();
  Mode::set("ModeReboot");
} // Task_WifiMgr::handle_do_reboot()

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
  html += WEB_NAME;
  html += "</div>";
  html += "<div style='font-size:x-large; color:#00F; background-color: #DDD;'>";
  html += title;
  html += "</div>";
  html += "<hr>";
  return html;
} // Task_WifiMgr::html_header()

/** static
 *
 */
std::string Task_WifiMgr::html_footer() {
  std::string html = "";
  html += "</body>";
  html += "</html>\n";
  return html;
} // Task_WifiMgr::html_footer();
