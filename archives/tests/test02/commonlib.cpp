/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "commonlib.h"

/**
 *
 */
uint8_t* get_mac_addr(uint8_t mac_addr[6]) {
  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  return mac_addr;
} // get_mac_addr()


/**
 *
 */
char* get_mac_addr_str(char mac_str[13]) {
  uint8_t mac_addr[6];

  get_mac_addr(mac_addr);
  sprintf(mac_str, "%02x%02x%02x%02x%02x%02x",
          mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
  log_d("MacAddr=%s", mac_str);

  return mac_str;
} // get_mac_addr()

/**
 *
 */
String get_mac_addr_String() {
  char mac_str[13];

  return String(get_mac_addr_str(mac_str));
}


/**
 *
 */
void task_delay(uint32_t ms) {
  //vTaskDelay(ms / portTICK_PERIOD_MS);

  //XXX TBD: vTaskDelay()は不正確？？
  portTickType lastWakeTime = xTaskGetTickCount();
  vTaskDelayUntil( &lastWakeTime, ms / portTICK_PERIOD_MS );
} // task_delay()
