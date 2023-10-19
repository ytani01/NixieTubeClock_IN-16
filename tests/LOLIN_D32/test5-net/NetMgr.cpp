/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "NetMgr.h"

#undef CONFIG_LOG_MAXIMUM_LEVEL
#define CONFIG_LOG_MAXIMUM_LEVEL 5

static ConfFile_Ssid *confSsid;
/**
 * Initialize static variables
 */
String NetMgr::myName = "NetMgr";
int16_t NetMgr::ssidN = 0;
SSIDent NetMgr::ssidEnt[NetMgr::SSID_N_MAX];
WebServer NetMgr::web_svr(WEBSVR_PORT);

static int try_connect_count = NetMgr::TRY_CONNECT_COUNT_MAX;

/** constructor
 *
 */
NetMgr::NetMgr(String ap_ssid_hdr) {
  if ( ap_ssid_hdr.length() > 0 ) {
    this->ap_ssid_hdr = ap_ssid_hdr;
  }

  esp_read_mac(this->mac_addr, ESP_MAC_WIFI_STA);
  char mac_str[13];
  sprintf(mac_str, "%02X%02X%02X%02X%02X%02X",
          this->mac_addr[0], this->mac_addr[1], this->mac_addr[2],
          this->mac_addr[3], this->mac_addr[4], this->mac_addr[5]);
  log_d("MacAddr=%s", mac_str);

  this->ap_ssid = this->ap_ssid_hdr + "_" + get_mac_addr_String();
  this->ap_ip = IPAddress(this->ap_ip_int[0],
                          this->ap_ip_int[1],
                          this->ap_ip_int[2],
                          this->ap_ip_int[3]);
  this->ap_netmask = IPAddress(this->ap_netmask_int[0],
                               this->ap_netmask_int[1],
                               this->ap_netmask_int[2],
                               this->ap_netmask_int[3]);

  confSsid = new ConfFile_Ssid;
} // NetMgr::NetMgr()

/**
 *
 */
NetMgrMode_t NetMgr::loop() {
  static NetMgrMode_t prev_mode = NETMGR_MODE_NULL;
  static String ssid = "";
  static String pw = "";
  static unsigned int wait_connect_count = NetMgr::WAIT_CONNECT_COUNT_MAX;

  if ( this->cur_mode != prev_mode ) {
    log_d("%d:%s ==> %d:%s",
          prev_mode, NETMGR_MODE_STR[prev_mode],
          this->cur_mode, NETMGR_MODE_STR[this->cur_mode]);
    prev_mode = this->cur_mode;
  }
  this->_loop_count++;

  wl_status_t wl_stat = WiFi.status();
  
  switch (this->cur_mode) {
  case NETMGR_MODE_NULL: {
    log_w("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);
    break;
  }
    
  case NETMGR_MODE_START: {
    log_d("NETMGR_MODE_START: try_connect_count=%d/%d",
          try_connect_count, NetMgr::TRY_CONNECT_COUNT_MAX);

    // 登録されているSSID,passwordを読み込む
    int ent_size = confSsid->load();
    if ( ent_size == 0 ) {
      log_w("confSsid->ent_size=%d", ent_size);

      try_connect_count = NetMgr::TRY_CONNECT_COUNT_MAX;
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }
    
    /*
     * XXX TBD 以下、悪あがき
     */
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);
    delay(100);
    
    // scan SSIDs
    log_d("scan SSID ..");

    uint16_t ssid_n = WiFi.scanNetworks();
    log_d("ssid_n=%d", ssid_n);

    // SSIDが無い場合は、APモード
    if ( ssid_n <= 0 ) {
      try_connect_count--;
      if ( try_connect_count <= 0 ) {
        this->cur_mode = NETMGR_MODE_AP_INIT;
      }
      break;
    }

    // SSIDエントリの数を制限
    if ( ssid_n > SSID_N_MAX ) {
      ssid_n = SSID_N_MAX;
      log_d("ssid_n=%d", ssid_n);
    }

    // ssidEnt[]に格納
    SSIDent ssid_ent[NetMgr::SSID_N_MAX];

    for (int i=0; i < ssid_n; i++) {
      ssid_ent[i].set(WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i));
      log_d("  %s", ssid_ent[i].toString().c_str());
    } // for()
    WiFi.scanDelete();

    String ssid = "";
    String pw = "";
    
    for (int i=0; i < ssid_n; i++) {
      auto itr = confSsid->ent.find(ssid_ent[i].ssid().c_str());

      if ( itr != confSsid->ent.end() )  {
        // found SSID
        ssid = String((itr->first).c_str());
        pw = String((itr->second).c_str());
        log_i("found:|%s|%s|", ssid.c_str(), pw.c_str());
        break;
      } // for(itr)
    } // for(i)

    if ( ssid == "" ) {
      log_w("not found");
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }
      
    // try to connect
    WiFi.begin(ssid.c_str(), pw.c_str());

    wait_connect_count = WAIT_CONNECT_COUNT_MAX;
    this->cur_mode = NETMGR_MODE_WAIT_CONNECT;
    break;
  } // NETMGR_MODE_START

  case NETMGR_MODE_WAIT_CONNECT: {
    log_d("%s %d/%d %d/%d wl_stat=%d:%s",
          NETMGR_MODE_STR[this->cur_mode],
          try_connect_count, NetMgr::TRY_CONNECT_COUNT_MAX,
          wait_connect_count, NetMgr::WAIT_CONNECT_COUNT_MAX,
          wl_stat, WL_STATUS_T_STR[wl_stat]);

    if (wl_stat == WL_CONNECTED) {
      // 接続
      this->ip_addr = WiFi.localIP();
      log_d("IPaddr=%s", this->ip_addr.toString().c_str());

      try_connect_count = NetMgr::TRY_CONNECT_COUNT_MAX; // XXX 不要？
      this->cur_mode = NETMGR_MODE_WIFI_ON;
      break;
    }
    
    // 未接続
    if ( wait_connect_count == 0 ) {
      // timeout
      try_connect_count--;

      if ( try_connect_count > 0 ) {
        // retry connect
        log_w("Retry ..");

        this->cur_mode = NETMGR_MODE_START;
        break;
      }

      // WiFi failed -> AP mode
      log_w(" WiFi faild");

      try_connect_count = NetMgr::TRY_CONNECT_COUNT_MAX;
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }

    // check restart_flag
    if ( this->restart_flag ) {
      log_d("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
      break;
    }

    // wait
    wait_connect_count--;
    delay(NetMgr::WAIT_CONNECT_INTERVAL);
    break;
  }

  case NETMGR_MODE_WIFI_ON:
    if ( wl_stat != WL_CONNECTED ) {
      log_w("wl_stat=%s(%d)", WL_STATUS_T_STR[wl_stat], wl_stat);

      try_connect_count = TRY_CONNECT_COUNT_MAX;
      this->cur_mode = NETMGR_MODE_START;
      break;
    }

    if ( this->restart_flag ) {
      log_d("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
    }
    break;

  case NETMGR_MODE_AP_INIT:
    // log_d("%s", this->ModeStr[this->cur_mode]);
    log_d("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);

    WiFi.disconnect(true); // 重要:以前の接続情報を削除
    WiFi.mode(WIFI_OFF);
    delay(100);

    WiFi.mode(WIFI_AP);
    log_d("WiFi.softAP: %s .. ", this->ap_ssid.c_str());
    delay(100);

    if ( ! WiFi.softAP(this->ap_ssid.c_str()) ) {
      log_d(" .. failed");
      WiFi.mode(WIFI_OFF);

      this->cur_mode = NETMGR_MODE_WIFI_OFF;
      break;
    }

    log_d(" .. start");
    delay(300);

    WiFi.softAPConfig(this->ap_ip, this->ap_ip, this->ap_netmask);

    this->dns_svr.setErrorReplyCode(DNSReplyCode::NoError);
    this->dns_svr.start(DNS_PORT, "*", this->ap_ip);
    log_d("DNS server[%d] started", DNS_PORT);

    NetMgr::async_scan_ssid_start();

    web_svr.enableDelay(false); // Important!!
    web_svr.on("/", NetMgr::handle_top);
    web_svr.on("/select_ssid", NetMgr::handle_select_ssid);
    web_svr.on("/save_ssid", NetMgr::handle_save_ssid);
    web_svr.on("/scan_ssid", NetMgr::handle_do_scan);
    web_svr.on("/confirm_reboot", NetMgr::handle_confirm_reboot);
    web_svr.on("/do_reboot", NetMgr::handle_do_reboot);
    web_svr.onNotFound(NetMgr::handle_top);
    web_svr.begin();
    log_d("Web server[%d] started", WEBSVR_PORT);

    this->cur_mode = NETMGR_MODE_AP_LOOP;

    break;

  case NETMGR_MODE_AP_LOOP:
    this->dns_svr.processNextRequest();
    web_svr.handleClient();

    if ( this->restart_flag ) {
      log_d("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
    }
    break;

  case NETMGR_MODE_WIFI_OFF:
    if (wl_stat == WL_CONNECTED) {
      log_d("wl_stat=%s(%d)", WL_STATUS_T_STR[wl_stat], wl_stat);
      this->cur_mode = NETMGR_MODE_WIFI_ON;
    }
    break;

  default:
    log_d("unknown mode ???");
    delay(1000);
    break;
  } // switch

  if ( this->cur_mode == NETMGR_MODE_WIFI_ON
       || this->cur_mode == NETMGR_MODE_WAIT_CONNECT ) {
    this->cur_ssid = ssid;
  } else {
    this->cur_ssid = "";
  }

  return this->cur_mode;
} // NetMgr::loop()

/** public
 *
 */
void NetMgr::save_ssid(String ssid, String pw) {
  log_d("save_ssid> |%s|%s|", ssid.c_str(), pw.c_str());

  confSsid->ent[ssid.c_str()] = pw.c_str();
  confSsid->save();
} // NetMgr::save_ssid()

/**
 *
 */
void NetMgr::restart() {
  log_d("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);
  this->restart_flag = true;
} // NetMgr::restart()

/** protected
 *
 */
void NetMgr::_restart() {
  log_d("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);
  this->restart_flag = false;
  this->cur_mode = NETMGR_MODE_START;

  try_connect_count = NetMgr::TRY_CONNECT_COUNT_MAX;
} // NetMgr::restart()

/**
 *
 */
String NetMgr::html_header(String title) {
  String html = "";
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
  html += NetMgr::myName;
  html += "</div>";
  html += "<div style='font-size:x-large; color:#00F; background-color: #DDD;'>";
  html += title;
  html += "</div>";
  html += "<hr>";
  return html;
} // NetMgr::handle_header()

/**
 *
 */
String NetMgr::html_footer() {
  String html = "";
  html += "</body>";
  html += "</html>\n";
  return html;
} // NetMgr::html_footer();

/**
 *
 */
void NetMgr::async_scan_ssid_start() {
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  //delay(200);

  WiFi.scanNetworks(true, true);
  log_d("WiFi.scanNetworks()");
} // NetMgr::async_scan_ssid_start()

/**
 *
 */
unsigned int NetMgr::async_scan_ssid_wait() {
  int16_t ret;

  while ( (ret = WiFi.scanComplete()) == WIFI_SCAN_RUNNING ) {
    log_d("scanning.. ret=%d", ret);
    delay(1000);
  }
  if ( ret == WIFI_SCAN_FAILED ) {
    log_e("SSID scan faild: ret=%d", ret);
    return 0;
  }

  if ( ret > NetMgr::SSID_N_MAX ) {
    ret = NetMgr::SSID_N_MAX;
  }

  for (int i=0; i < ret; i++) {
    NetMgr::ssidEnt[i].set(WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i));
  } // for()

  WiFi.scanDelete();

  return ret;
} // NetMgr::async_scan_ssid_wait()

/**
 *
 */
void NetMgr::handle_top() {
  String   ssid = "", pw = "";
  
  log_d("");

  confSsid = new ConfFile_Ssid;
  if ( confSsid->load() > 0 ) {
    ssid = confSsid->ent.begin()->first.c_str();
    pw = confSsid->ent.begin()->second.c_str();
  }
  log_d("ssid=%s, pw=%s", ssid.c_str(), pw.c_str());

  String html = NetMgr::html_header("Current settings");
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
  html += NetMgr::html_footer();
  web_svr.send(200, "text/html", html);
} // NetMgr::handle_top()

/**
 *
 */
void NetMgr::handle_select_ssid() {
  String   ssid = "", pw = "";
  
  if ( confSsid->load() > 0 ) {
    ssid = confSsid->ent.begin()->first.c_str();
    pw = confSsid->ent.begin()->second.c_str();
  }
  log_d("ssid=%s, pw=%s", ssid.c_str(), pw.c_str());
  
  log_d("scan SSIDs ...");
  uint16_t ssid_n = WiFi.scanNetworks();
  log_d("ssid_n=%s", ssid_n);

  if (ssid_n <= 0) {
    log_d("rescan SSID..");
    uint16_t ssid_n = WiFi.scanNetworks();
    log_d("ssid_n = %s", ssid_n);
  }

  if ( ssid_n > NetMgr::SSID_N_MAX ) {
    ssid_n = NetMgr::SSID_N_MAX;
    log_d("ssid_n = %d", ssid_n);
  }
  if ( ssid_n < 0 ) {
    ssid_n = 0;
    log_d("ssid_n = %d", ssid_n);
  }

  SSIDent ssid_ent[NetMgr::SSID_N_MAX];

  for (int i=0; i < ssid_n; i++) {
    ssid_ent[i].set(WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i));
    log_d("  %s", ssid_ent[i].toString().c_str());
  }
  WiFi.scanDelete();
  
  String html = NetMgr::html_header("Please change settings and save");

  html += "<form action='/save_ssid' method='GET'>";
  html += "<div class='ssid'>";
  html += "SSID ";
  html += "<select name='ssid' id='ssid' style='font-size:large;'>";

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
  html += NetMgr::html_footer();

  web_svr.send(200, "text/html", html);
} // NetMgr::handle_select_ssid()

/**
 *
 */
void NetMgr::handle_save_ssid(){
  String ssid = web_svr.arg("ssid");
  String pw = web_svr.arg("passwd");
  
  log_d("save_ssid> |%s|%s|", ssid.c_str(), pw.c_str());

  confSsid->ent[ssid.c_str()] = pw.c_str();
  confSsid->save();

  // 自動転送
  web_svr.sendHeader("Location", String("/"), true);
  web_svr.send(302, "text/plain", "");
} // NetMgr::handle_save_ssid()

/**
 *
 */
void NetMgr::handle_confirm_reboot() {
  String html = NetMgr::html_header("Reboot confirmation");
  html += "<p>Are you sure to reboot ";
  html += NetMgr::myName;
  html += " ?</p>\n";
  html += "<a href='/do_reboot'>Yes</a>";
  html += " or ";
  html += "<a href='/'>No</a>";
  html += NetMgr::html_footer();
  web_svr.send(200, "text/html", html.c_str());
} // NetMgr::handle_confirm_reboot()

/**
 *
 */
void NetMgr::handle_do_scan() {
  NetMgr::async_scan_ssid_start();
  NetMgr::async_scan_ssid_wait();

  // 自動転送
  web_svr.sendHeader("Location", String("/select_ssid"), true);
  web_svr.send(302, "text/plain", "");
} // NetMgr::handle_do_rescan()

/**
 *
 */
void NetMgr::handle_do_reboot() {
  String html = NetMgr::html_header("Rebooting ..");
  html += "Please reconnect WiFi ..";
  html += "<hr />";
  html += NetMgr::html_footer();
  web_svr.send(200, "text/html", html.c_str());

  log_d("reboot esp32 ..");
  delay(2000);
  ESP.restart();
} // NetMgr::handle_do_reboot()
