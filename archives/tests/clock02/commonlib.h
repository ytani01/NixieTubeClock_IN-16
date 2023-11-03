/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_

#include <Arduino.h>
#include <esp_system.h>
#include <RTClib.h>

String get_mac_addr_String();

char *tm2str(struct tm *tm);
char *datetime2str(DateTime *dt);

#endif // _COMMONLIB_H_
