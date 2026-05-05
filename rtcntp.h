//=====================================================================================
// RTC synchronized with NTP server
//=====================================================================================
#pragma once

#include <time.h>

//-------------------------------------------------------------------------------------
// To save Flash size, use the following string instead of `strftime()`
//-------------------------------------------------------------------------------------
#define DAYS_OF_THE_WEEK    { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" }
#define MONTHS_OF_THE_YEAR  { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }

//-------------------------------------------------------------------------------------
// To save heap memory, use 32bit unsigned long instead of 64bit unsigned long long
//-------------------------------------------------------------------------------------
typedef uint32_t time32_t;

//-------------------------------------------------------------------------------------
// Function prototype declaration
//-------------------------------------------------------------------------------------
void rtcInit(void);
bool rtcSyncNTP(void);
void rtcSetTimeZoneOffset(int32_t offset);
bool rtcCurrentTime (time32_t *time);
void rtcGetLocalTime(time32_t time, struct tm *tm);
void rtcPrintTime   (time32_t time);  // Apr 01 (Sun) 2026 HH:MM:SS
String rtcStringTime(time32_t time);  // HH:MM:SS
String rtcStringWeek(time32_t time);  // SUN, MON, ...
String rtcStringDate(time32_t time);  // Apr 01 WED HH:MM