/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_

#include <Arduino.h>
#include <esp_system.h>
#include <esp32-hal-log.h>
#include <RTClib.h>
#include <chrono>

/**
 * memory leak check
 */
bool chk_memleak(int sec, int unit=10);

/**
 * MAC address
 */
std::string get_mac_addr_string();

/**
 * date time
 */
static const int DATETIME_STR_LEN = 128;

DateTime tm2datetime(struct tm *tm);

std::string tm2string(struct tm *tm,
                      const char fmt[]="%Y/%m/%d(%a) %H:%M:%S");

std::string datetime2string(DateTime *dt,
                            const char fmt[]="%Y/%m/%d(%a) %H:%M:%S");

bool is_leap_year(int year);
int last_day(int year, int month);

/**
 * class name
 */
inline std::string className(const std::string& prettyFunction)
{
    size_t colons = prettyFunction.find("::");
    if (colons == std::string::npos)
        return "::";
    size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
    size_t end = colons - begin;

    return prettyFunction.substr(begin,end);
}
#define __CLASS_NAME__ className(__PRETTY_FUNCTION__)

#endif // _COMMONLIB_H_
