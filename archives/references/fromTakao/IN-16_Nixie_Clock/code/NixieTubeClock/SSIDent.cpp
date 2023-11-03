/**
 * (c) 2021 Yoichi Tanibayashi
 */
#include "SSIDent.h"

/**
 *
 */
SSIDent::SSIDent() {
} // SSIDent::SSIDent()

/**
 *
 */
SSIDent::SSIDent(String ssid, int dbm, wifi_auth_mode_t enc_type) {
  this->set(ssid, dbm, enc_type);
} // SSIDent::SSIDent()

/**
 *
 */
void SSIDent::set(String ssid, int dbm, wifi_auth_mode_t enc_type) {
  this->_ssid = ssid;
  this->_dbm = dbm;
  this->_enc_type = enc_type;
}

/**
 *
 */
String SSIDent::encTypeStr(wifi_auth_mode_t enc_type) {
  String enc_type_str = "";
  switch (enc_type) {
  case WIFI_AUTH_OPEN: enc_type_str = "open"; break;
  case WIFI_AUTH_WEP: enc_type_str = "WEP"; break;
  case WIFI_AUTH_WPA_PSK: enc_type_str = "WPA_PSK"; break;
  case WIFI_AUTH_WPA2_PSK: enc_type_str = "WPA2_PSK"; break;
  case WIFI_AUTH_WPA_WPA2_PSK: enc_type_str = "WPA_WPA2_PSK"; break;
  case WIFI_AUTH_WPA2_ENTERPRISE: enc_type_str = "WPA2_ENTERPRISE"; break;
  default: enc_type_str = "???"; break;
  }

  return enc_type_str;
} // SSIDent::encTypeStr()

/**
 *
 */
String SSIDent::ssid() {
  return this->_ssid;
} // SSIDent::ssid()

/**
 *
 */
int SSIDent::dbm() {
  return this->_dbm;
} // SSIDent::dbm()

/**
 *
 */
String SSIDent::encType() {
  return SSIDent::encTypeStr(this->_enc_type);
} // SSIDent::encType()

/**
 *
 */
String SSIDent::toString(boolean flag_ssid, boolean flag_dbm, boolean flag_enctype) {
  String str = "";
  if ( flag_ssid ) {
    str += this->_ssid + " ";
  }
  if ( flag_dbm ) {
    str += String(this->_dbm) + "dBm ";
  }
  if ( flag_enctype ) {
    str += SSIDent::encTypeStr(this->_enc_type) + " ";
  }
  str.trim();

  return str;
} // SSIDent::toString()
