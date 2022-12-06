#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;


// Update via WiFi or GSM - uncomment to use WiFi
#define WIFI_UPDATES
#define TINY_GSM_MODEM_SIM808

// Version
string ESP32_GSM_FIRMWARE_VERSION; 

// ACCESS POINT
string ESP32_GSM_APN;
string ESP32_GSM_GPRS_USER;
string ESP32_GSM_GPRS_PASSWORD;

// SERVER
string ESP32_GSM_HOST;
uint16_t ESP32_GSM_PORT;
string ESP32_GSM_OAUTH_TOKEN_PATH;
string ESP32_GSM_RESOURCE_PATH;

// FIRWARE UPDATE
string ESP32_GSM_UPDATE_HOST;
string ESP32_GSM_UPDATE_URL; // must include either http:// or https:// for WiFi
string ESP32_GSM_UPDATE_VERSION_FILE_URL;

// WIFI CREDENTIALS
string ESP32_GSM_WIFI_SSID;
string ESP32_GSM_WIFI_PASSWORD;

#endif