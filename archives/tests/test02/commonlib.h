/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_

#include <Arduino.h>
#include <esp_system.h>

uint8_t* get_mac_addr(uint8_t mac_addr[6]);
char*  get_mac_addr_str(char mac_str[13]);
String get_mac_addr_String();

void task_delay(uint32_t ms);

#endif // _COMMONLIB_H_
