//=====================================================================================
// RTC synchronized with NTP server
//=====================================================================================
#include <Arduino.h>
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "rtcntp.h"
#include "gfx.h"

static const char *week [] = DAYS_OF_THE_WEEK;
static const char *month[] = MONTHS_OF_THE_YEAR;

static const char *ntpServers[] = {
  "ntp.nict.jp",
  "ntp.jst.mfeed.ad.jp",
  "pool.ntp.org",
  "time.google.com",
//"europe.pool.ntp.org",
//"time.cloudflare.com",
//"time.aws.com",
};

#if defined(ARDUINO_UNOR4_WIFI)

#include <RTC.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// NTP servers
static int serverID = 0;
#define NTP_N_SERVERS (sizeof(ntpServers) / sizeof(ntpServers[0]))

// Time zone offset for RTC (only for UNO R4 WiFi)
static int32_t timezoneOffset = NTP_TIMEZONE_OFFSET;

//-------------------------------------------------------------------------------------
// Initialize the RTC and configure synchronization with NTP server
//-------------------------------------------------------------------------------------
bool rtcInit(void) {
  gfxDrawMessage("Syncing RTC with NTP...");

  // Initialize real time clock
  RTC.begin();

  if (rtcSyncNTP()) {
    gfxDrawMessage("done.\n", false);
    return true;
  } else {
    gfxDrawMessage("timeout.\n", false);
    return false;
  }
}

//-------------------------------------------------------------------------------------
// Synchronize the RTC with the NTP server
//-------------------------------------------------------------------------------------
bool rtcSyncNTP(void) {
  DBG_EXEC(Serial.println("Connecting to " + String(ntpServers[serverID]) + "..."));

  // https://docs.arduino.cc/tutorials/uno-r4-wifi/rtc/
  // https://github.com/arduino-libraries/NTPClient
  // https://github.com/arduino/ArduinoCore-renesas/tree/main/libraries/RTC/examples/RTC_NTPSync
  WiFiUDP Udp;

  bool stat = false;
  do {
    // Connect UDP to the NTP default port
    NTPClient timeClient(Udp, ntpServers[serverID], NTP_TIMEZONE_OFFSET, NTP_SYNC_INTERVAL);
    timeClient.begin();

    if (timeClient.update()) {
      // Get the current unix time from an NTP server and set to RTC
      auto unixTime = timeClient.getEpochTime();
      RTCTime rtcTime = RTCTime(unixTime);

      // Set Daylight Savint Time
      if (NTP_TIMEZONE_DST && RTC.getTime(rtcTime)) {
        rtcTime.setSaveLight(SaveLight::SAVING_TIME_ACTIVE);
      }

      RTC.setTime(rtcTime);

      DBG_EXEC(Serial.println("The RTC was set to " + rtcTime.toString()));
      stat = true;   // RTC update successful
    } else {
      DBG_EXEC(Serial.println("failed to connect."));
    }

    // Stop UDP and NTPClient
    timeClient.end();

    // Set up the following server
    serverID = (serverID + 1) % NTP_N_SERVERS;
    const char *server = ntpServers[serverID];
    DBG_EXEC(Serial.println("Next NTP server: " + String(server)));
  } while (stat == false);

  return true;
}

#else // ESP32

#include <esp_sntp.h>

static bool synchronized = false;

//-------------------------------------------------------------------------------------
// Notification callback function on synchronizing NTP server
//
// struct timeval {
//   time_t      tv_sec;  /* seconds */
//   suseconds_t tv_usec; /* microseconds */
// };
//-------------------------------------------------------------------------------------
static void syncNTP_cb(struct timeval *tv) {
  if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
    struct tm timeInfo;
    localtime_r(&tv->tv_sec, &timeInfo);

    DBG_EXEC(Serial.print("The RTC was just set to: "));
    DBG_EXEC(Serial.println(&timeInfo, "%Y-%m-%d %H:%M:%S"));
    synchronized = true;
  }
}

//-------------------------------------------------------------------------------------
// Initialize the RTC and configure synchronization with NTP server
//-------------------------------------------------------------------------------------
bool rtcInit(void) {
  gfxDrawMessage("Syncing RTC with NTP...");

  // https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Time/SimpleTime
  // https://github.com/espressif/esp-idf/blob/master/components/lwip/include/apps/esp_sntp.h
  // https://github.com/espressif/esp-idf/blob/master/components/lwip/apps/sntp/sntp.c
  sntp_set_sync_interval(NTP_SYNC_INTERVAL);
  sntp_set_time_sync_notification_cb(syncNTP_cb);

  // A more convenient approach to handle Time Zone with daylight offset specifying
  // a environment variable with TimeZone definition including daylight adjustment rules.
  // A list of rules for your zone could be obtained from:
  // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c#L81-L113
  configTzTime(NTP_TIMEZONE_STRING, ntpServers[0], ntpServers[1], ntpServers[2]);

  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    gfxDrawMessage("done.\n", false);
    return true;
  } else {
    gfxDrawMessage("timeout.\n", false);
    return false;
  }
}

bool rtcSyncNTP(void) {
  if (synchronized) {
    synchronized = false;
    return true;
  }
  return false;
}

#endif // ARDUINO_UNOR4_WIFI or ESP32

//-------------------------------------------------------------------------------------
// Set time zone offset in seconds
//-------------------------------------------------------------------------------------
void rtcSetTimeZoneOffset(int32_t offset) {
#if defined(ARDUINO_UNOR4_WIFI)
  timezoneOffset = offset;
#endif
}

//-------------------------------------------------------------------------------------
// Get current local time
//
// struct timeval {
//   time_t      tv_sec;  /* seconds */
//   suseconds_t tv_usec; /* microseconds */
// };
//-------------------------------------------------------------------------------------
time32_t rtcCurrentTime(void) {
#if defined(ARDUINO_UNOR4_WIFI)

  // https://github.com/arduino/ArduinoCore-renesas/blob/main/libraries/RTC/src/RTC.cpp
  #if   false
    RTCTime currentTime;
    RTC.getTime(currentTime); // Returns false if the RTC has not been initialized
    return (time32_t)currentTime.getUnixTime();
  #else
    struct timeval tv;
    gettimeofday(&tv, NULL);  // Always returns 0 in ArduinoCore-renesas
    return (time32_t)tv.tv_sec;
  #endif

#else // ESP32

  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c
  return (time32_t)time(NULL);

#endif
}

//-------------------------------------------------------------------------------------
// Convert UTC time to local time
//
// struct tm {
//   int	tm_sec;   // [0-61]
//   int	tm_min;   // [0-59]
//   int	tm_hour;  // [0-23]
//   int	tm_mday;  // [1-31]
//   int	tm_mon;   // [0-11]
//   int	tm_year;  // years since 1900
//   int	tm_wday;  // [0:Sun 1:Mon ... 6:Sat]
//   int	tm_yday;  // [0-365]
//   int	tm_isdst; // Daylight Saving Time　[DST > 0, DST == 0 or DST < 0]
// };
//-------------------------------------------------------------------------------------
void rtcConvtLocalTime(time32_t time, struct tm *tm) {
#if defined(ARDUINO_UNOR4_WIFI)
  // On UNO R4, a value of time zone offset needs to be added
  time += timezoneOffset;
#endif

  time_t t = (time_t)time;
  localtime_r(&t, tm);
}

//-------------------------------------------------------------------------------------
// Convert local time to a string representing the date
//-------------------------------------------------------------------------------------
String rtcStringDate(time32_t time) {
  struct tm tm;
  time_t t = (time_t)time;
  localtime_r(&t, &tm);

  char buf[32];
  snprintf(buf, sizeof(buf), "%s %02d %s %02d:%02d",
    month[tm.tm_mon], tm.tm_mday, week[tm.tm_wday], tm.tm_hour, tm.tm_min
  );
  return String(buf);
}

//-------------------------------------------------------------------------------------
// Convert local time to a string
//-------------------------------------------------------------------------------------
String rtcStringTime(time32_t time) {
  struct tm tm;
  time_t t = (time_t)time;
  localtime_r(&t, &tm);

  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  return String(buf);
}

#if DEBUG
//-------------------------------------------------------------------------------------
// Print the UTC time from the JSON data as a string.
//-------------------------------------------------------------------------------------
void rtcPrintLocalTime(time32_t time) {
  struct tm tm;
  rtcConvtLocalTime(time, &tm);

  char buf[32];
  snprintf(buf, sizeof(buf), "%s %02d (%s) %d %02d:%02d:%02d",
    month[tm.tm_mon], tm.tm_mday, week[tm.tm_wday], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec
  );
  Serial.println(buf);
}
#endif // DEBUG

//-------------------------------------------------------------------------------------
// Convert UTC time to a string representing the day of the week
//-------------------------------------------------------------------------------------
const char *rtcLocalDayOfWeek(time32_t time) {
  struct tm tm;
  rtcConvtLocalTime(time, &tm);
  return week[tm.tm_wday];
}

//-------------------------------------------------------------------------------------
// Convert UTC time to a string representing the day of the week
//-------------------------------------------------------------------------------------
const char *rtcGetDayOfWeek(int wday) {
  return week[wday];
}