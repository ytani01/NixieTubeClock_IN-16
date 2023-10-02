/**
 * (c) 2021 Yoichi Tanibayashi
 */
#ifndef SSID_ENT_H
#define SSID_ENT_H

#include <WiFi.h>

/**
 *
 */
class SSIDent {
 public:
  SSIDent();
  SSIDent(String ssid, int dbm, wifi_auth_mode_t enc_type);
  
  void set(String ssid, int dbm, wifi_auth_mode_t enc_type);

  static String encTypeStr(wifi_auth_mode_t enc_type);

  String ssid();
  int dbm();
  String encType();
  
  String toString(boolean f_ssid = true,
                  boolean f_dbm = false,
                  boolean f_enctype = false);
  
 private:
  String _ssid;
  int _dbm;
  wifi_auth_mode_t _enc_type;
};

#endif // SSID_ENT_H
