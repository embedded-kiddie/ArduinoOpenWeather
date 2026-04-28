//=====================================================================================
// RTC functions synchronized with NTP server
//=====================================================================================
#include <Arduino.h>
#include <stdio.h>
#include "config.h"
#include "rtcntp.h"

// NTP servers
static const char *ntpServers[] = {
  "ntp.nict.jp",
  "ntp.jst.mfeed.ad.jp",
  "pool.ntp.org",
  "time.google.com",
  "time.cloudflare.com",
  "time.aws.com",
};

static const char *week [] = DAYS_OF_THE_WEEK;
static const char *month[] = MONTHS_OF_THE_YEAR;

#if defined(ARDUINO_UNOR4_WIFI)

#include <RTC.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define NTP_UPDATE_INTERVAL (3 * 60 * 1000)
static int32_t timezoneOffset = TIMEZONE_OFFSET;

// A single instance to let us send and receive packets over UDP
static WiFiUDP Udp;

#else // ESP32

#include <esp_sntp.h>

#endif

//-------------------------------------------------------------------------------------
// Initialize the RTC and configure synchronization with NTP server
//-------------------------------------------------------------------------------------
void rtcInit(void) {
#if defined(ARDUINO_UNOR4_WIFI)

  // Initialize real time clock
  RTC.begin();
  rtcUpdate();

#else // ESP32

  // https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Time/SimpleTime
  // https://github.com/espressif/esp-idf/blob/master/components/lwip/include/apps/esp_sntp.h
  // https://github.com/espressif/esp-idf/blob/master/components/lwip/apps/sntp/sntp.c
  sntp_set_sync_interval(UPDATE_INTERVAL);
  sntp_set_time_sync_notification_cb(rtcUpdate);

  // A more convenient approach to handle Time Zone with daylight offset specifying 
  // a environment variable with TimeZone definition including daylight adjustment rules.
  // A list of rules for your zone could be obtained from:
  // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c#L81-L113
  configTzTime(TIMEZONE_STRING, ntpServers[0], ntpServers[1], ntpServers[2]);

  Serial.print("Waiting for RTC to be synchronized with NTP server...");
  struct tm timeInfo;
  getLocalTime(&timeInfo);

#endif
}

#if defined(ARDUINO_UNOR4_WIFI)
//-------------------------------------------------------------------------------------
// Synchronize the RTC with the NTP server
//-------------------------------------------------------------------------------------
void rtcUpdate(void) {
  static uint32_t lastUpdate;
  if (lastUpdate == 0 || millis() - lastUpdate >= NTP_UPDATE_INTERVAL) {
    Serial.println("Connecting to NTP server...");

    // https://docs.arduino.cc/tutorials/uno-r4-wifi/rtc/
    // https://github.com/arduino-libraries/NTPClient
    // https://github.com/arduino/ArduinoCore-renesas/tree/main/libraries/RTC/examples/RTC_NTPSync
    for (int i = 0; i < sizeof(ntpServers) / sizeof(ntpServers[0]); i++) {
      Serial.println(ntpServers[i]);

      // By default 'pool.ntp.org' is used with 60 seconds update interval and no offset.
      // You can specify the time server pool and the offset, (in seconds)
      // additionally you can specify the update interval (in milliseconds).
      // e.g. NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
      NTPClient timeClient(Udp, ntpServers[i], timezoneOffset);
  
      timeClient.begin();
      if (timeClient.update()) {
        // Get the current date and time from an NTP server and convert it
        // to 'UTC+X' by passing 'X' as the time zone offset in hours.
        // ToDo: Adjusting Daylight Saving Time (https://docs.arduino.cc/libraries/timezone/)
        auto unixTime = timeClient.getEpochTime();
        RTCTime timeToSet = RTCTime(unixTime);
        RTC.setTime(timeToSet);

        // Retrieve the date and time from the RTC and print them
        RTCTime currentTime;
        RTC.getTime(currentTime);
        Serial.println("The RTC was just set to: " + currentTime.toString());

        lastUpdate = millis();
        break;
      }
    }
  }
}

#else // ESP32
//-------------------------------------------------------------------------------------
// Notification callback function on synchronizing NTP server
//
// struct timeval {
//   time_t      tv_sec;  /* seconds */
//   suseconds_t tv_usec; /* microseconds */
// };
//-------------------------------------------------------------------------------------
void rtcUpdate(struct timeval *tv) {
  if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
    struct tm timeInfo;
    localtime_r(&tv->tv_sec, &timeInfo);

    Serial.print("The RTC was just set to: ");
    Serial.println(&timeInfo, "%Y-%m-%d %H:%M:%S");
  }
}
#endif

//-------------------------------------------------------------------------------------
// Set time zone offset in seconds
//-------------------------------------------------------------------------------------
void rtcSetTimeZoneOffset(int32_t offset) {
#if defined(ARDUINO_UNOR4_WIFI)
  timezoneOffset = offset;
#endif
}

//-------------------------------------------------------------------------------------
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
bool rtcCurrentTime(time32_t *t) {
#if defined(ARDUINO_UNOR4_WIFI)

  // https://github.com/arduino/ArduinoCore-renesas/blob/main/libraries/RTC/src/RTC.cpp
  RTCTime currentTime;
  if (RTC.getTime(currentTime)) {
    *t = (time32_t)currentTime.getUnixTime();
    return true;    
  }
  return false;

#else // ESP32

// https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c
  time_t now = time(NULL);
  *t = (time32_t)now;
  return true;

#endif
}

//-------------------------------------------------------------------------------------
// Convert "time32_t" (4bytes) to "struct tm" (thread-safe)
//-------------------------------------------------------------------------------------
void rtcConvertTime(time32_t time, struct tm *tm) {
#if defined(ARDUINO_UNOR4_WIFI)
  // On UNO R4, it seems the time zone offset set by NTPClient is not reflected.
  time += timezoneOffset;
#endif

  time_t t = (time_t)time;
  localtime_r(&t, tm);
}

//-------------------------------------------------------------------------------------
// Convert "struct tm" to a String [ Alternative function for "strftime()" ]
//-------------------------------------------------------------------------------------
String rtcWeekString(time32_t time) {
  struct tm tm;
  rtcConvertTime(time, &tm);
  return String(week[tm.tm_wday]);
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
String rtcDateString(time32_t time) {
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
//
//-------------------------------------------------------------------------------------
String rtcTimeString(time32_t time) {
  struct tm tm;
  time_t t = (time_t)time;
  localtime_r(&t, &tm);

  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  return String(buf);
}

void rtcPrintTime(time32_t time) {
  struct tm tm;
  rtcConvertTime(time, &tm);

  char buf[32];
  snprintf(buf, sizeof(buf), "%s %02d (%s) %d %02d:%02d:%02d",
    month[tm.tm_mon], tm.tm_mday, week[tm.tm_wday], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec
  );
  Serial.println(buf);
}