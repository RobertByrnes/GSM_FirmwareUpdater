#include <Arduino.h>
#include <modem.h>
// #include <CRC32.h>
#include <Update.h>
// #include "FS.h"
// #include "SPIFFS.h"
// #include "FSImpl.h"
#include <hexdump.cpp>

// #define LOGGING  // <- Logging is for the HTTP library

// #define FORMAT_SPIFFS_IF_FAILED true

// uint32_t knownCRC32 = 0xCE664544; // 0x32B6E001
// uint32_t knownFileSize = 308688;  // In case server does not send it 838260 / 838681
uint totalLength = 0;
uint currentLength = 0;
// uint32_t spiffsPartitionSize = 0;

// Your GPRS credentials, if any
const char apn[] = "giffgaff.com";
const char gprsUser[] = "gg";
const char gprsPass[] = "p";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "CrowdedHouse";
const char wifiPass[] = "kF4QMhzc3xcS";

// Server details
const char server[] = "draperbiotech.clystnet.com";
const char resource[] = "/firmware.bin";
const int port = 443;

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClientSecure client(modem);
HttpClient http(client, server, port);


void processUpdate(uint8_t *data, size_t len, uint32_t contentLength)
{
  Update.write(data, len);
  currentLength += len;
  Serial.printf("[Current Length] %u\n", currentLength);
  // Print dots while waiting for update to finish
  Serial.print('.');
  // if current length of written firmware is not equal to total firmware size, repeat
  if (currentLength != contentLength) return;
  // Update.end(true);
  // Serial.printf("\n[+] Update Success, Total Size: %u\nRebooting...\n", currentLength); 
  
  // client.stop();
  // Serial.println(F("[i] Server disconnected"));

  // modem.gprsDisconnect();
  // Serial.println(F("[i] GPRS disconnected"));

  // ESP.restart(); // Restart ESP32 to see changes
}

void setup()
{
  // Set console baud rate
  Serial.begin(115200);

  // spiffsInit();

  delay(10);

  // SIM800 pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);


  // Set GSM module baud rate
  // TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX); // Set GSM module baud rate and UART pins
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("[i] Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("[i] Modem Info: ");
  Serial.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif
}

void loop()
{
#if TINY_GSM_USE_WIFI
  // Wifi connection parameters must be set before waiting for the network
  Serial.print(F("[i] Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    Serial.println("[-] fail");
    delay(10000);
    return;
  }
  Serial.println("[+] success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  Serial.print("[i] Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println("[-] fail");
    delay(10000);
    return;
  }
  Serial.println("[+] success");

  if (modem.isNetworkConnected()) { Serial.println("[+] Network connected"); }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  Serial.printf("[+] Connecting to %s", apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" ... fail");
    delay(10000);
    return;
  }
  Serial.println(" ... success");

  if (modem.isGprsConnected()) { Serial.println("[+] GPRS connected"); }
#endif

  Serial.printf("[i] Connecting to %s", server);
  if (!client.connect(server, port)) {
    Serial.println(" ... fail");
    delay(10000);
    return;
  }
  Serial.println(" ... success");

  // Make a HTTP GET request:
  client.print(String("GET ") + resource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();

  Serial.println(F("[+] Waiting for response header"));

  // While we are still looking for the end of the header (i.e. empty line
  // FOLLOWED by a newline), continue to read data into the buffer, parsing each
  // line (data FOLLOWED by a newline). If it takes too long to get data from
  // the client, we need to exit.

  uint32_t clientReadTimeout = 20000L;
  uint32_t clientReadStartTime = millis();
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;

  while (!finishedHeader) {
    int nlPos;

    if (client.available()) {
      clientReadStartTime = millis();
      while (client.available()) {
        char c = client.read();

        // Uncomment to view hexDump of incoming header
        // uint32_t c = client.read();   
        // hexDump(Serial, "HEXDUMP: ", &c, 32, 16); 
        headerBuffer += c;

        // Uncomment the lines below to see the data coming into the buffer
        // if (c < 16)
        //   Serial.print('0');
        // Serial.print(c, HEX);
        // Serial.print(' ');
        // if (isprint(c))
        //   Serial.print(reinterpret_cast<char> (c));
        // else
        //   Serial.print('*');
        // Serial.print(' ');

        // Let's exit and process if we find a new line
        if (headerBuffer.indexOf(F("\r\n")) >= 0) break;
      }
    } else {
      if (millis() - clientReadStartTime > clientReadTimeout) {
        // Time-out waiting for data from client
        Serial.println(F("[-] Client Timeout !"));
        break;
      }
    }

    // See if we have a new line.
    nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0) {
      headerBuffer.toLowerCase();
      // Check if line contains content-length
      if (headerBuffer.startsWith(F("content-length:"))) {
        contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        Serial.print(F("[+] Got Content Length: "));  // uncomment for confirmation
        Serial.println(contentLength);
      }

      if (headerBuffer.startsWith(F("x-powered-by:"))) {
        Serial.print(F("[+] Got Host: ")); // uncomment for confirmation of last line of the header
        Serial.println(headerBuffer);
      }

      headerBuffer.remove(0, nlPos + 2);  // remove the line
    } else if (nlPos == 0) {
      // if the new line is empty (i.e. "\r\n" is at the beginning of the line),
      // we are done with the header.
      finishedHeader = true;
    }
  }

  uint32_t readLength = 0;

  // if (finishedHeader && contentLength == knownFileSize) {
  if (finishedHeader && contentLength) {
    Serial.println(F("[i] Reading response data"));
    clientReadStartTime = millis();

    //     // Uncomment to view hexDump of incoming binary file 
    //     hexDump(Serial, "HEXDUMP: ", &c, 8, 8); 


    clientReadTimeout = 180000L;
    uint32_t len = contentLength;
    uint8_t buff[128] = { 0 }; // create buffer for read
    Serial.println("[i] Updating firmware...");
    Update.begin(UPDATE_SIZE_UNKNOWN);

    while (client.connected() && (len > 0 || len == -1)) {
        size_t size = client.available(); // get available data size
        Serial.printf("[Size] %u\n", size);
        if (size) {
          int c = client.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size)); // read up to 128 byte
          processUpdate(buff, c, contentLength);

          if (len > 0) {
              len -= c;
          }
          delay(1);
        }
    }
  }

  // Shutdown

  Update.end(true);
  Serial.printf("\n[+] Update Success, Total Size: %u\nRebooting...\n", currentLength); 
  
  client.stop();
  Serial.println(F("[i] Server disconnected"));

  modem.gprsDisconnect();
  Serial.println(F("[i] GPRS disconnected"));

  ESP.restart(); // Restart ESP32 to see changes
}