#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;


// Update via WiFi or GSM - uncomment to use WiFi
// #define WIFI_UPDATES

// Version
const string CURRENT_VERSION = "0.0.1"; 

// Hardware ISR timer_1 setup
// #define TIMER_1_CONNECTION_INTERVAL 3600000000L // every hour
// #define TIMER_1_CONNECTION_INTERVAL 3000000000L // every 50mins
// #define TIMER_1_CONNECTION_INTERVAL 2400000000L // every 40mins
// #define TIMER_1_CONNECTION_INTERVAL 1800000000L // every 30mins
// #define TIMER_1_CONNECTION_INTERVAL 1200000000L// every 20mins
// #define TIMER_1_CONNECTION_INTERVAL 900000000L // every 15mins
// #define TIMER_1_CONNECTION_INTERVAL 600000000L // every 10mins
// #define TIMER_1_CONNECTION_INTERVAL 300000000L // every 5mins
// #define TIMER_1_CONNECTION_INTERVAL 180000000L // every 3mins
// #define TIMER_1_CONNECTION_INTERVAL 120000000L // every 2mins
// #define TIMER_1_CONNECTION_INTERVAL 60000000L // every 1min
// #define TIMER_1_CONNECTION_INTERVAL 30000000L // every 30sec
#define TIMER_1_CONNECTION_INTERVAL 10000000L // every 10sec
// #define TIMER_1_CONNECTION_INTERVAL 1000000L // every 1sec

// Hardware ISR timer_2 setup
// #define TIMER_2_CONNECTION_INTERVAL 3600000000L // every hour
// #define TIMER_2_CONNECTION_INTERVAL 3000000000L // every 50mins
// #define TIMER_2_CONNECTION_INTERVAL 2400000000L // every 40mins
// #define TIMER_2_CONNECTION_INTERVAL 1800000000L // every 30mins
// #define TIMER_2_CONNECTION_INTERVAL 1200000000L // every 20mins
// #define TIMER_2_CONNECTION_INTERVAL 900000000L // every 15mins
// #define TIMER_2_CONNECTION_INTERVAL 600000000L // every 10mins
// #define TIMER_2_CONNECTION_INTERVAL 300000000L // every 5mins
// #define TIMER_2_CONNECTION_INTERVAL 180000000L // every 3mins
// #define TIMER_2_CONNECTION_INTERVAL 120000000L // every 2mins
#define TIMER_2_CONNECTION_INTERVAL 60000000L // every 1min
// #define TIMER_2_CONNECTION_INTERVAL 30000000L // every 30sec
// #define TIMER_2_CONNECTION_INTERVAL 10000000L // every 10sec
// #define TIMER_2_CONNECTION_INTERVAL 1000000L // every 1sec

// GPRS credentials
const char *APN = "giffgaff.com"; // APN Access Point Name
const char *GPRS_USER = "gg"; // GPRS User
const char *GPRS_PASSWORD = "p"; // GPRS Password

// End point details for the server
const char SERVER[] = "draperbiotech.clystnet.com"; // Base url
const char AUTH_PATH[] = "/oauth/token"; // oAuth token path
const char RESOURCE_PATH[] = "/api/readings"; // Resource path
const uint16_t PORT = 443; // Server port number

// OAuth Token
const string OAUTH_HOST = "draperbiotech.clystnet.com";
const string OAUTH_TOKEN_PATH = "/oauth/token";

// location of firmware file on external web server
// change to your actual .bin location
const char *UPDATE_HOST = "draperbiotech.clystnet.com";
const char *UPDATE_URL = "draperbiotech.clystnet.com/firmware.bin"; // must include either http:// or https:// for WiFi
const char *UPDATE_VERSION_FILE_URL = "https://draperbiotech.clystnet.com/firmware.txt";

// WiFi credentials
const char *SSID = "CrowdedHouse"; // WiFi Network Name
const char *PASSWORD = "kF4QMhzc3xcS"; // WiFi Network Password

#endif