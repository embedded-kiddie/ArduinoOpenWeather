//=====================================================================================
// A class definition for OpenWeather
// https://openweathermap.org/forecast5?collection=current_forecast
//=====================================================================================
#include <Arduino.h>
#include <ArduinoJson.h>

#include "config.h"
#include "rtcntp.h"
#include "OpenWeather.h"
#include "root_ca.hpp"

// Select the libraries to use
#define USE_SECURE_CLIENT true
#define USE_HTTP_CLIENT   false

// Select a method for deserializing JSON data
#define TYPE_STATIC_DAT 0 // response_forecast.hpp
#define TYPE_GET_STRING 1 // Output to Serial Monitor
#define TYPE_RAW_STREAM 2 // Deserialize by ArduinoJson

#define DESERIALIZATION_TYPE  TYPE_RAW_STREAM

// Format of an HTTP request to OpenWeatherMap
static constexpr char host[] = HOST;
static constexpr char path[] = PATH "?lang=" LANGUAGE "&lat=" LATITUDE "&lon=" LONGITUDE "&units=" UNITS "&appid=" API_KEY;

#if defined(ARDUINO_UNOR4_WIFI)

  #include "WiFiS3.h"

  #if USE_SECURE_CLIENT
    WiFiSSLClient client;
    #define PORT  443
  #else
    WiFiClient client;
    #define PORT  80
  #endif

  #if USE_HTTP_CLIENT
    #include <HttpClient.h>
    HttpClient http(client, host, PORT);
  #endif

#else // ESP32

  #if USE_SECURE_CLIENT
    #include <NetworkClientSecure.h>
    NetworkClientSecure client;
    #define PORT  443
  #else
    #include <NetworkClient.h>
    NetworkClient client;
    #define PORT  80
  #endif

  #if USE_HTTP_CLIENT
    #include <HTTPClient.h>
    HTTPClient http;
  #endif

#endif

#if defined(ESP32)
//-------------------------------------------------------------------------------------
// Extend the deserialization limit to 16KB or more.
// https://arduinojson.org/v7/how-to/use-external-ram-on-esp32/
//-------------------------------------------------------------------------------------
struct HeapAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_DEFAULT);
  }
  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }
  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_DEFAULT);
  }
};
#endif // ESP32

//-------------------------------------------------------------------------------------
// Initialization
//-------------------------------------------------------------------------------------
void OpenWeather::Init(void) {
  // Configure the root CA certificate
#if USE_SECURE_CLIENT
  #ifdef _ROOT_CA_CERTIFICATE_HPP_
    client.setCACert(root_ca);
  #elif defined(ESP32)
    client.setInsecure(); // ignore server's certificate
  #endif
#endif
}

//-------------------------------------------------------------------------------------
// HTTP request
//-------------------------------------------------------------------------------------
bool OpenWeather::RequestWeatherData(void) {
  bool ret = false;

#if (DESERIALIZATION_TYPE == TYPE_STATIC_DAT)

  // Pass 'Serial' as a dummy argument
  ret = readResponse(Serial);

#elif (USE_HTTP_CLIENT)

  Serial.print("\nConnecting to ");
  Serial.print(String(host) + ":" + String(PORT));
  Serial.print("...");

  #if defined(ARDUINO_UNOR4_WIFI)

  // Send HTTP request
  int status = http.get(path);

  // https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/src/HTTPClient.h#L46-L123
  Serial.print("GET status: ");
  Serial.println(status);

  if (status == HTTP_SUCCESS) {
    // Read status code in response
    status = http.responseStatusCode();
    Serial.print("HTTP status: ");
    Serial.println(status);

    if (200 <= status && status < 300) {
      // Read response header section
      if (http.skipResponseHeaders() == HTTP_SUCCESS) {
        // Read response body section
        ret = readResponse(http);
      } else {
        Serial.println("Can't read respones body.");
      }
    }
  }

  http.stop();  // Disconnect
  Serial.println("done.");

  #else // USE_HTTP_CLIENT for ESP32

    // Initialize the SSL library
    #if USE_SECURE_CLIENT
      String scheme = "https://";
    #else
      String scheme = "http://";
    #endif

  http.begin(client, scheme + host + path);
  int status = http.GET();

  // https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/src/HTTPClient.h#L46-L123
  Serial.printf("HTTP status: %d %s\n", status, http.errorToString(status).c_str());

  if (status == HTTP_CODE_OK || status == HTTP_CODE_MOVED_PERMANENTLY) {
    Stream &stream = http.getStream();
    ret = readResponse(stream);
  }

  http.end();  // Disconnect
  Serial.println("done.");

  #endif // USE_HTTP_CLIENT for UNO R4 WiFi or ESP32

#else // ! USE_HTTP_CLIENT for UNO R4 WiFi and ESP32

  Serial.print("\nConnecting to ");
  Serial.print(String(host) + ":" + String(PORT));
  Serial.print("...");

  if (!client.connect(host, PORT)) {
    Serial.println("Connection failed!");
    client.stop();
    return false;
  }

  Serial.println("connected.");

  // Send HTTP request
  // Note: If the header "Transfer-Encoding" is "chunked", use "HTTP/1.0".
  // https://arduinojson.org/v7/how-to/use-arduinojson-with-httpclient/
  client.println(String("GET ") + path + " HTTP/1.1");
  client.println(String("Host: ") + host);
  client.println("Connection: close");
  client.println();

  // Read response headers until "\r\n\r\n" is detected.
  bool detected = false;
  while (client.connected()) {
    String header = client.readStringUntil('\n');
    Serial.println(header);
    if (header == "\r") {
      Serial.println("End of headers.");
      detected = true;
      break;
    }
  }

  // Read response body section
  if (detected && client.available()) {
    ret = readResponse(client);
  } else {
    Serial.println("Invalid response.");
  }

  client.stop();
  Serial.println("done.");

#endif

  return ret;
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
static constexpr char filter_template[] = R"(
  {
    "cnt": true,
    "list": [
      {
        "dt": true,
        "main": {
          "temp": true,
          "humidity": true
        },
        "weather": [
          {
            "id": true
          }
        ],
        "clouds": {
          "all": true
        },
        "wind": {
          "speed": true,
          "deg": true
        },
        "pop": true
      }
    ],
    "city": {
      "timezone": true,
      "sunrise": true,
      "sunset": true
    }
  }
)";

//-------------------------------------------------------------------------------------
// Read HTTP response body and build JsonDocument
//-------------------------------------------------------------------------------------
bool OpenWeather::readResponse(Stream &stream) {
#if (DESERIALIZATION_TYPE == TYPE_STATIC_DAT)
  #include "samples/response_forecast.hpp"
#else
  Stream &response = stream;
#endif

#if (DESERIALIZATION_TYPE == TYPE_GET_STRING) 

  while (response.available()) {
    char c = response.read();
    Serial.write(c);
  }
  Serial.println();
  return true;

#else

  JsonDocument filter;
  deserializeJson(filter, filter_template);

  DeserializationError error = deserializeJson(data, response, DeserializationOption::Filter(filter));
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return false;
  }

  serializeJson(data, Serial); Serial.println();
//serializeJsonPretty(data, Serial); Serial.println();
  return true;

#endif
}