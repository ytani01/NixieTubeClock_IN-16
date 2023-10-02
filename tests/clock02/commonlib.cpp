/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "common.h"

String get_mac_addr_String() {
  uint8_t mac_addr[6];
  char mac_str[13];

  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  sprintf(mac_str, "%02x%02x%02x%02x%02x%02x",
          mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
  log_i("MacAddr=%s", mac_str);

  return String(mac_str);
} // get_mac_addr_String()
