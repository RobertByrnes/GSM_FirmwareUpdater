#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;

// Main
string ESP32_GSM_FIRMWARE_VERSION; 

// BOARD CONFIG
int ESP32_GSM_BAUD_RATE = 115200;
uint8_t ESP32_GSM_LED_PIN;
uint8_t ESP32_GSM_MODEM_POWER_PIN;
uint8_t ESP32_GSM_MODEM_RESET_PIN;
uint8_t ESP32_GSM_MODEM_POWER_ON_PIN;
uint8_t ESP32_GSM_MODEM_RX_PIN;
uint8_t ESP32_GSM_MODEM_TX_PIN;

// ACCESS POINT
string ESP32_GSM_APN = "pkp18-inet";
string ESP32_GSM_GPRS_USER = "";
string ESP32_GSM_GPRS_PASSWORD = "";
string ESP32_GSM_SIM_PIN;

// SERVER
string ESP32_GSM_OAUTH_TOKEN_PATH;
string ESP32_GSM_RESOURCE_PATH;

// FIRWARE UPDATE
uint32_t ESP32_GSM_STREAM_TIMEOUT;
uint16_t ESP32_GSM_PORT;
string ESP32_GSM_UPDATE_HOST;
string ESP32_GSM_UPDATE_URL; // must include either http:// or https:// for WiFi
string ESP32_GSM_UPDATE_VERSION_FILE_URL;

// WIFI CREDENTIALS
string ESP32_GSM_WIFI_SSID;
string ESP32_GSM_WIFI_PASSWORD;

#endif