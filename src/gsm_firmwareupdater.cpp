#include <Arduino.h>
#include <modem.h>
#include <CRC32.h>
#include <Update.h>
#include "FS.h"
#include "SPIFFS.h"
#include "FSImpl.h"
#include <hexdump.cpp>

// #define LOGGING  // <- Logging is for the HTTP library

#define FORMAT_SPIFFS_IF_FAILED true

uint32_t knownCRC32 = 0xCE664544; // 0x32B6E001
uint32_t knownFileSize = 308688;  // In case server does not send it 838260 / 838681
uint totalLength = 0;
uint currentLength = 0;
uint32_t spiffsPartitionSize = 0;

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

void writeUpdate(uint8_t *data, size_t len)
{
  size_t written = Update.write(data, len);
  Serial.print("Written: ");
  Serial.println(written);
  currentLength += len;
  Serial.print("Current Length: ");
  Serial.println(currentLength);
  Serial.print("Total Length: ");
  Serial.println(totalLength);
  // Print dots while waiting for update to finish
  Serial.print('.');
  // if current length of written firmware is not equal to total firmware size, repeat
  if (currentLength != totalLength)
    return;
  Update.end(true);
  Serial.printf("\nUpdate Success, Total Size: %u\nRebooting...\n", currentLength);
  // Restart ESP32 to see changes
  ESP.restart();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("[i] Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  [i] File: ");
      Serial.print(file.name());
      Serial.print("  Size: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

bool spiffsInit() // public
{
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("[i] SPIFFS Mount Failed");
    return false;
  }
  
  Serial.println("[i] SPIFFS Mounted");
  SPIFFS.format();
  spiffsPartitionSize = SPIFFS.totalBytes();
  Serial.printf("[i] SPIFFS drive size available, %u\n", spiffsPartitionSize);
  listDir(SPIFFS, "/", 0);

  return true;
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);

  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  if (file.print(message)) {
    Serial.println("APOK");
  } else {
    Serial.println("APX");
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  
  while (file.available()) {
    Serial.write(file.read());
    delayMicroseconds(100);
  }
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void beginProcessingUpdate(Stream &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    // size_t written = Update.write(updateSource);
    if (written == updateSize) {
      Serial.println("[i] Written : " + String(written) + " successfully");
    } else {
      Serial.println("[-] Only written : " + String(written) + "/" + String(updateSize) + ". Retry?");
      Serial.println(Update.errorString());
    }

    if (Update.end()) {
      Serial.println("[+] OTA complete!");
      if (Update.isFinished()) {
        Serial.println("[+] Ota successful, restarting!");
        ESP.restart();
      } else {
        Serial.println("[-] Ota did not complete, something went wrong!");
      }
    } else {
      Serial.println("[-] Error Occured #: " + String(Update.getError()));
    }
  } else {
    Serial.println("[-] Not enough space to do OTA");
  }
}

void updateFromFS()
{
  File updateBin = SPIFFS.open("/update.bin");
  if (updateBin) {
    if (updateBin.isDirectory()) {
      Serial.println("[-] Error, in the directory");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      Serial.println("[i] Trying to start update");
      beginProcessingUpdate(updateBin, updateSize);
    } else {
      Serial.println("[-] Error, empty file");
    }

    updateBin.close();

    // whe finished remove the binary from sd card to indicate end of the process
    //fs.remove("/update.bin");
  } else {
    Serial.println("Can't upload file");
  }
}

void setup()
{
  // Set console baud rate
  Serial.begin(115200);

  spiffsInit();

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

void printPercent(uint32_t readLength, uint32_t contentLength) {
  // If we know the total length
  if (contentLength != (uint32_t)-1) {
    Serial.print("\r ");
    Serial.printf("[\% Downloaded] %u", (100.0 * readLength) / contentLength);
    Serial.print('%');
  } else {
    Serial.println(readLength);
  }
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
  Serial.print(F("[+] Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("[-] fail");
    delay(10000);
    return;
  }
  Serial.println("[-] success");

  if (modem.isGprsConnected()) { Serial.println("[+] GPRS connected"); }
#endif

  Serial.print(F("[!] Connecting to "));
  Serial.print(server);
  if (!client.connect(server, port)) {
    Serial.println("[-] fail");
    delay(10000);
    return;
  }
  Serial.println("[+] success");

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

  const uint32_t clientReadTimeout = 20000L;
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

  // The two cases which are not managed properly are as follows:
  // 1. The client doesn't provide data quickly enough to keep up with this
  // loop.
  // 2. If the client data is segmented in the middle of the 'Content-Length: '
  // header,
  //    then that header may be missed/damaged.
  //

  uint32_t readLength = 0;
  CRC32 crc;
  File file = SPIFFS.open("/update.bin", FILE_APPEND);
  Serial.println("[+] File opened");

  if (finishedHeader && contentLength == knownFileSize) {
    Serial.println(F("[i] Reading response data"));
    clientReadStartTime = millis();

    printPercent(readLength, contentLength);
    while (readLength < contentLength && client.connected() && millis() - clientReadStartTime < clientReadTimeout) {
      while (client.available()) {
        uint32_t c = client.read();   

        // Uncomment to view hexDump of incoming binary file 
        // hexDump(Serial, "HEXDUMP: ", &c, 32, 16); 

        if (!file.print(c)) {
          Serial.println("[-] Append Fault");
          Serial.println(SPIFFS.usedBytes());
        }
        // Serial.print(c);  // Uncomment this to show
        // data
        // crc.update(c);
        readLength++;
        if (readLength % (contentLength / 13) == 0) {
          printPercent(readLength, contentLength);
        }
        clientReadStartTime = millis();
      }
    }
    printPercent(readLength, contentLength);
  }

  timeElapsed = millis() - timeElapsed;
  Serial.println();

  // Shutdown

  client.stop();
  Serial.println(F("[i] Server disconnected"));

#if TINY_GSM_USE_WIFI
  modem.networkDisconnect();
  Serial.println(F("[i] WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
  modem.gprsDisconnect();
  Serial.println(F("[i] GPRS disconnected"));
#endif

  float duration = float(timeElapsed) / 1000;

  Serial.println();
  Serial.print("[i] Content-Length: ");
  Serial.println(contentLength);
  Serial.print("[i] Actually read:  ");
  Serial.println(readLength);
  Serial.print("[i] Calc. CRC32:    0x");
  Serial.println(crc.finalize(), HEX);
  Serial.print("[i] Known CRC32:    0x");
  Serial.println(knownCRC32, HEX);
  Serial.print("[i] Duration:       ");
  Serial.print(duration);
  Serial.println("s");

  updateFromFS();

  // Do nothing forevermore
  while (true) { delay(1000); }
}