#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using namespace std;


// Version
const string CURRENT_VERSION = "0.0.1"; 

// Hardware ISR timer_1 setup
// #define TIMER_1_CONNECTION_INTERVAL 3600000000U // every hour
// #define TIMER_1_CONNECTION_INTERVAL 3000000000U // every 50mins
// #define TIMER_1_CONNECTION_INTERVAL 2400000000U // every 40mins
// #define TIMER_1_CONNECTION_INTERVAL 1800000000U // every 30mins
// #define TIMER_1_CONNECTION_INTERVAL 1200000000U // every 20mins
// #define TIMER_1_CONNECTION_INTERVAL 900000000U // every 15mins
// #define TIMER_1_CONNECTION_INTERVAL 600000000U // every 10mins
// #define TIMER_1_CONNECTION_INTERVAL 300000000U // every 5mins
// #define TIMER_1_CONNECTION_INTERVAL 180000000U // every 3mins
// #define TIMER_1_CONNECTION_INTERVAL 120000000U // every 2mins
// #define TIMER_1_CONNECTION_INTERVAL 60000000U // every 1min

// Hardware ISR timer_2 setup
// #define TIMER_2_CONNECTION_INTERVAL 3600000000U // every hour
// #define TIMER_2_CONNECTION_INTERVAL 3000000000U // every 50mins
// #define TIMER_2_CONNECTION_INTERVAL 2400000000U // every 40mins
// #define TIMER_2_CONNECTION_INTERVAL 1800000000U // every 30mins
// #define TIMER_2_CONNECTION_INTERVAL 1200000000U // every 20mins
// #define TIMER_2_CONNECTION_INTERVAL 900000000U // every 15mins
// #define TIMER_2_CONNECTION_INTERVAL 600000000U // every 10mins
// #define TIMER_2_CONNECTION_INTERVAL 300000000U // every 5mins
// #define TIMER_2_CONNECTION_INTERVAL 180000000U // every 3mins
// #define TIMER_2_CONNECTION_INTERVAL 120000000U // every 2mins
#define TIMER_2_CONNECTION_INTERVAL 60000000U // every 1min

// Hardware ISR timer_3 setup
// #define TIMER_3_CONNECTION_INTERVAL 3600000000U // every hour
// #define TIMER_3_CONNECTION_INTERVAL 3000000000U // every 50mins
// #define TIMER_3_CONNECTION_INTERVAL 2400000000U // every 40mins
// #define TIMER_3_CONNECTION_INTERVAL 1800000000U // every 30mins
// #define TIMER_3_CONNECTION_INTERVAL 1200000000U // every 20mins
// #define TIMER_3_CONNECTION_INTERVAL 900000000U // every 15mins
// #define TIMER_3_CONNECTION_INTERVAL 600000000U // every 10mins
// #define TIMER_3_CONNECTION_INTERVAL 300000000U // every 5mins
// #define TIMER_3_CONNECTION_INTERVAL 180000000U // every 3mins
#define TIMER_3_CONNECTION_INTERVAL 120000000U // every 2mins
// #define TIMER_3_CONNECTION_INTERVAL 60000000U // every 1min

// GPRS credentials
const char *APN = "giffgaff.com"; // APN Access Point Name
const char *GPRS_USER = "gg"; // GPRS User
const char *GPRS_PASSWORD = "p"; // GPRS Password

// End point details for the server
const char SERVER[] = "server.com"; // Base url
const char AUTH_PATH[] = "/oauth/token"; // oAuth token path
const char RESOURCE_PATH[] = "/api/readings"; // Resource path
const uint16_t PORT = 443; // Server port number

// OAuth Token
const string OAUTH_HOST = "server.com";
const string OAUTH_TOKEN_PATH = "/oauth/token";

// location of firmware file on external web server
// change to your actual .bin location
const char *UPDATE_HOST = "server.com";
const char *UPDATE_URL = "https://server.com/firmware.bin"; // must include either http:// or https://
const char *UPDATE_VERSION_FILE_URL = "https://server.com/firmware.txt";

// WiFi credentials
const char *SSID = "ssid"; // WiFi Network Name
const char *PASSWORD = "password"; // WiFi Network Password

#endif