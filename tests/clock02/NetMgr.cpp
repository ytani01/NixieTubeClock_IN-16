/**
 * Copyright (c) 2021 Yoichi Tanibayashi
 */
#include "NetMgr.h"

#undef CONFIG_LOG_MAXIMUM_LEVEL
#define CONFIG_LOG_MAXIMUM_LEVEL 5

static ConfSsid *confSsid;
/**
 * Initialize static variables
 */
String NetMgr::myName = "NetMgr";
int16_t NetMgr::ssidN = 0;
SSIDent NetMgr::ssidEnt[NetMgr::SSID_N_MAX];
WebServer NetMgr::web_svr(WEBSVR_PORT);

/** constructor
 *
 */
NetMgr::NetMgr(String ap_ssid_hdr, unsigned int try_count_max) {
  if ( ap_ssid_hdr.length() > 0 ) {
    this->ap_ssid_hdr = ap_ssid_hdr;
  }
  if ( try_count_max > 0 ) {
    this->try_count_max = try_count_max;
  }

  esp_read_mac(this->mac_addr, ESP_MAC_WIFI_STA);
  char mac_str[13];
  sprintf(mac_str, "%02X%02X%02X%02X%02X%02X",
          this->mac_addr[0], this->mac_addr[1], this->mac_addr[2],
          this->mac_addr[3], this->mac_addr[4], this->mac_addr[5]);
  log_i("MacAddr=%s", mac_str);

  this->ap_ssid = this->ap_ssid_hdr + "_" + get_mac_addr_String();
  this->ap_ip = IPAddress(this->ap_ip_int[0],
                          this->ap_ip_int[1],
                          this->ap_ip_int[2],
                          this->ap_ip_int[3]);
  this->ap_netmask = IPAddress(this->ap_netmask_int[0],
                               this->ap_netmask_int[1],
                               this->ap_netmask_int[2],
                               this->ap_netmask_int[3]);

  confSsid = new ConfSsid;
} // NetMgr::NetMgr()

/**
 *
 */
NetMgrMode_t NetMgr::loop() {
  static NetMgrMode_t prev_mode = NETMGR_MODE_NULL;
  static String ssid = "";
  static String pw = "";
  static int retry_count = 2; // XXX WiFiが頻繁に切れるのでリトライ

  if ( this->cur_mode != prev_mode ) {
    log_i("cur_mode: %s(%d) ==> %s(%d)",
          NETMGR_MODE_STR[prev_mode], prev_mode,
          NETMGR_MODE_STR[this->cur_mode], this->cur_mode);
    prev_mode = this->cur_mode;
  }
  this->_loop_count++;

  wl_status_t wl_stat = WiFi.status();
  
  int ent_size = 0;

  switch (this->cur_mode) {
  case NETMGR_MODE_NULL:
    break;
    
  case NETMGR_MODE_START:
    log_i("NETMGR_MODE_START: retry_count=%d", retry_count);

    confSsid->load();
    ent_size = confSsid->ent.size();
    if ( ent_size == 0 ) {
      log_w("ent_size=%d", ent_size);
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }
    
    /*
     * XXX 以下、悪あがき
     * XXX TBD
     */
    WiFi.mode(WIFI_OFF);
    log_i("WIFI_OFF");
    delay(100);

    //eps_wifi_restore();
    //log_i("esp_wifi_restore()");
    //delay(5000);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);
    delay(100);
    
    // scan SSIDs
    log_i("scan SSID ..");
    NetMgr::ssidN = WiFi.scanNetworks(false, true);
    log_i("ssidN=%d", NetMgr::ssidN);

    if ( ssidN <= 0 ) {
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(200);

      log_i("retry: scan SSID ..");
      NetMgr::ssidN = WiFi.scanNetworks(false, true);
      log_i("ssidN=%d", NetMgr::ssidN);
    }
    if ( ssidN <= 0 ) {
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }

    if ( NetMgr::ssidN > SSID_N_MAX ) {
      NetMgr::ssidN = SSID_N_MAX;
      log_i("ssidN=%d", NetMgr::ssidN);
    }
    for (int i=0; i < NetMgr::ssidN; i++) {
      log_i("  %s", WiFi.SSID(i).c_str());
      NetMgr::ssidEnt[i].set(WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i));
    } // for()
    WiFi.scanDelete();

    ssid = "";
    for (int i=0; i < NetMgr::ssidN; i++) {
      auto itr = confSsid->ent.find(NetMgr::ssidEnt[i].ssid().c_str());

      if ( itr != confSsid->ent.end() )  {
        ssid = (itr->first).c_str();
        pw = (itr->second).c_str();
        log_i("found:|%s|%s|", ssid.c_str(), pw.c_str());
        break;
      } // for(itr)
    } // for(i)

    if ( ssid == "" ) {
      log_i("not found");
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }
      
    WiFi.begin(ssid.c_str(), pw.c_str());

    this->_loop_count = 0;
    this->cur_mode = NETMGR_MODE_TRY_WIFI;
    break;

  case NETMGR_MODE_TRY_WIFI:
    if ( this->restart_flag ) {
      log_i("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
      break;
    }

    if (wl_stat == WL_CONNECTED) {
      log_i("wl_stat=%s(%d): IPaddr=%s",
            WL_STATUS_T_STR[wl_stat], wl_stat,
            WiFi.localIP().toString().c_str());

      //WiFi.persistent(false);
      this->ip_addr = WiFi.localIP();
      this->net_is_available = true;
      this->cur_mode = NETMGR_MODE_WIFI_ON;
      break;
    }

    if (this->_loop_count > this->try_count_max) {
      if ( retry_count ) {
        log_w("Retry .. (%d)", retry_count);
        this->cur_mode = NETMGR_MODE_START;
        retry_count--;
        break;
      }
      
      log_w(" WiFi faild");
      this->cur_mode = NETMGR_MODE_AP_INIT;
      break;
    }

    log_w("%s %d/%d wl_stat=%s(%d)",
          NETMGR_MODE_STR[this->cur_mode],
          this->_loop_count, this->try_count_max,
          WL_STATUS_T_STR[wl_stat], wl_stat);

    delay(TRY_INTERVAL);
    break;

  case NETMGR_MODE_AP_INIT:
    retry_count = 2;

    // log_i("%s", this->ModeStr[this->cur_mode]);
    log_i("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);

    WiFi.disconnect(true); // 重要:以前の接続情報を削除
    WiFi.mode(WIFI_OFF);
    delay(100);

    WiFi.mode(WIFI_AP);
    log_i("WiFi.softAP: %s .. ", this->ap_ssid.c_str());
    delay(100);

    if ( ! WiFi.softAP(this->ap_ssid.c_str()) ) {
      log_i(" .. failed");
      WiFi.mode(WIFI_OFF);

      this->cur_mode = NETMGR_MODE_WIFI_OFF;
      break;
    }

    log_i(" .. start");
    delay(300);

    WiFi.softAPConfig(this->ap_ip, this->ap_ip, this->ap_netmask);

    this->dns_svr.setErrorReplyCode(DNSReplyCode::NoError);
    this->dns_svr.start(DNS_PORT, "*", this->ap_ip);
    log_i("DNS server[%d] started", DNS_PORT);

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
    log_i("Web server[%d] started", WEBSVR_PORT);

    this->cur_mode = NETMGR_MODE_AP_LOOP;

    break;

  case NETMGR_MODE_AP_LOOP:
    this->dns_svr.processNextRequest();
    web_svr.handleClient();

    if ( this->restart_flag ) {
      log_i("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
    }
    break;

  case NETMGR_MODE_WIFI_ON:
    retry_count = 2;

    if ( wl_stat != WL_CONNECTED ) {
      log_w("wl_stat=%s(%d)", WL_STATUS_T_STR[wl_stat], wl_stat);
      this->cur_mode = NETMGR_MODE_START;
      break;
    }

    if ( this->restart_flag ) {
      log_i("restart_flag=%s", (this->restart_flag ? "true" : "false"));
      this->_restart();
    }
    break;

  case NETMGR_MODE_WIFI_OFF:
    retry_count = 2;

    if (wl_stat == WL_CONNECTED) {
      log_i("wl_stat=%s(%d)", WL_STATUS_T_STR[wl_stat], wl_stat);
      this->cur_mode = NETMGR_MODE_WIFI_ON;
    }
    break;

  default:
    log_i("unknown mode ???");
    delay(1000);
    break;
  } // switch

  if ( this->cur_mode == NETMGR_MODE_WIFI_ON
       || this->cur_mode == NETMGR_MODE_TRY_WIFI ) {
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
  log_i("save_ssid> |%s|%s|", ssid.c_str(), pw.c_str());

  confSsid->ent[ssid.c_str()] = pw.c_str();
  confSsid->save();
} // NetMgr::save_ssid()

/**
 *
 */
void NetMgr::restart() {
  log_i("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);
  this->restart_flag = true;
} // NetMgr::restart()

/** protected
 *
 */
void NetMgr::_restart() {
  log_i("cur_mode=%s", NETMGR_MODE_STR[this->cur_mode]);
  this->restart_flag = false;
  this->cur_mode = NETMGR_MODE_START;

  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
  // delay(100);
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
  log_i("WiFi.scanNetworks()");
} // NetMgr::async_scan_ssid_start()

/**
 *
 */
unsigned int NetMgr::async_scan_ssid_wait() {
  int16_t ret;

  while ( (ret = WiFi.scanComplete()) == WIFI_SCAN_RUNNING ) {
    log_i("scanning.. ret=%d", ret);
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
  String   ssid, pw;

  confSsid->load();
  ssid = confSsid->ent.begin()->first.c_str();
  pw = confSsid->ent.begin()->second.c_str();
  log_i("ssid=%s, pw=%s", ssid.c_str(), pw.c_str());

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
  String   ssid, pw;
  

  confSsid->load();
  //ssid = "";
  //pw = "";
  
  NetMgr::ssidN = NetMgr::async_scan_ssid_wait();
  log_i("ssidN=%s", String(NetMgr::ssidN));
  if (NetMgr::ssidN == 0) {
    log_i("rescan SSID..");
    NetMgr::async_scan_ssid_start();
    NetMgr::ssidN = NetMgr::async_scan_ssid_wait();
  }

  for (int i=0; i < NetMgr::ssidN; i++) {
    log_i("[%2d]%4ddBm %s (%s)", i,
          NetMgr::ssidEnt[i].dbm(),
          NetMgr::ssidEnt[i].ssid().c_str(),
          NetMgr::ssidEnt[i].encType().c_str());
  } // for(i)

  String html = NetMgr::html_header("Please change settings and save");

  html += "<form action='/save_ssid' method='GET'>";
  html += "<div class='ssid'>";
  html += "SSID ";
  html += "<select name='ssid' id='ssid' style='font-size:large;'>";

  for(int i=0; i < NetMgr::ssidN; i++){
    html += "<option value=" + NetMgr::ssidEnt[i].ssid();
    if ( NetMgr::ssidEnt[i].ssid() == ssid ) {
      html += " selected";
    }
    html += ">";
    html += NetMgr::ssidEnt[i].ssid();
    /*
    html += " (";
    html += String(NetMgr::ssidEnt[i].dbm());
    html += ", ";
    html += NetMgr::ssidEnt[i].encType();
    html += ")";
    */
    html += "</option>\n";
  } // for(i)

  html += "<option value="">(clear)</option>\n";
  html += "</select><br />\n";

  html += "Password ";
  html += "<span style='font-size: xx-large'>";
  html += "<input type='password'";
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
  
  log_i("save_ssid> |%s|%s|", ssid.c_str(), pw.c_str());

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

  log_i("reboot esp32 ..");
  delay(2000);
  ESP.restart();
} // NetMgr::handle_do_reboot()
