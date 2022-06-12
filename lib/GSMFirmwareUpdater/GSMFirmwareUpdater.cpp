#include "GSMFirmwareUpdater.h"


// Constructor
GSMFirmwareUpdater::GSMFirmwareUpdater(const char *UPDATE_URL, const char *UPDATE_HOST, const uint16_t &PORT):
  totalLength(0), currentLength(0), updateUrl(UPDATE_URL), updateHost(UPDATE_HOST), port(PORT)  {}

// Destructor
GSMFirmwareUpdater::~GSMFirmwareUpdater() {}

const char *GSMFirmwareUpdater::cleanUrl(const char *url)
{
  std::string string = url;
  int index = string.find_first_of(":");
  Serial.printf("[D] Index of : %u\n", index);
  int indexZero = string.find_first_of("h");
  Serial.printf("[D] Index of : %u\n", indexZero);

  if (index == 5) {
    string.replace(0, 8, "");
  } else if (index == 4) {
    string.replace(0, 7, "");
  }

  return string.c_str();
}

bool GSMFirmwareUpdater::connectNetowrk(CellularNetwork800L &network) // private
{
  if (!network.connectNetwork()) {
    return false;
  }
  return true;
}

void GSMFirmwareUpdater::printPercent(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != -1) {
    Serial.print("\r ");
    Serial.print((100.0 * readLength) / contentLength);
    Serial.println('%');
  } else {
    Serial.println(readLength);
  }
}

/**
 * @brief Function to update firmware incrementally.
 * Buffer is declared to be 128 so chunks of 128 bytes
 * from firmware is written to device until server closes.
 * 
 * @param UPDATE_HOST const char *
 * @param PORT uint16_t
 * @param http HttpClient
 * @param client TinyGsmClientSecure
 * 
 * @return void
 */ 
void GSMFirmwareUpdater::updateFirmware(TinyGsmClientSecure &client, CellularNetwork800L &network)
{  
  if (!this->connectNetowrk(network)) {
    return;
  }

  uint32_t   knownCRC32    = 0x6f50d767;
  uint32_t   knownFileSize = 1024;  // In case server does not send it

  if (!client.connect(this->updateHost, this->port)) {
    Serial.print("[-] Connecting to update server failed: Url\n");
    Serial.println(this->updateHost);
    Serial.println(this->port);
    Serial.print(this->updateUrl);

    return;
    
  } else {
    Serial.println("[+] Connected to update server");
  }

  Serial.print("[i] Performing HTTPS GET request to ");
  Serial.println(this->updateUrl);

  // Make a HTTP GET request:
  client.print(String("GET ") + this->updateUrl + " HTTP/1.0\r\n");
  client.print(String("Host: ") + this->updateHost + "\r\n");
  client.print("Connection: close\r\n\r\n");
  Serial.println(F("[i] Waiting for response header"));

  long timeout = millis();
  while (client.available() == 0) {
      if (millis() - timeout > 5000L) {
          Serial.println("[-] Client Timeout");
          client.stop();
          delay(10000L);
          return;
      }
  }

  Serial.println("[D] Header received");
  uint32_t contentLength = knownFileSize;

  File file = SPIFFS.open("/update.bin", FILE_APPEND);

  while (client.available()) {
    String line = client.readStringUntil('\n');
    line.trim();
    //Serial.println(line);    // Uncomment this to show response header
    line.toLowerCase();
    if (line.startsWith("content-length:")) {
        contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
    } else if (line.length() == 0) {
        break;
    }
  }

  Serial.println("[D] receiving response");
  timeout = millis();
  uint32_t readLength = 0;
  CRC32 crc;

  unsigned long timeElapsed = millis();
  this->printPercent(readLength, contentLength);

  while (readLength < contentLength && client.connected() && millis() - timeout < 10000L) {
    Serial.println("[D] connected");
    int i = 0;
    char c;
    while (client.available()) {
    Serial.println("[D] available");
      char c;
      c = (char)client.read();
      Serial.println(c); 
      if (!file.print(c)) {
        Serial.println("[-] Append Fault");
      }
      Serial.println(c);       // Uncomment this to show data
      //crc.update(c);
      readLength++;

      if (readLength % (contentLength / 13) == 0) {
        printPercent(readLength, contentLength);
      }
      timeout = millis();
    }
  }

  file.close();

  printPercent(readLength, contentLength);
  timeElapsed = millis() - timeElapsed;

  client.stop();
  Serial.println("[-] Server disconnected");

  network.connection.gprsDisconnect();
  Serial.println("[-] GPRS disconnected");

  float duration = float(timeElapsed) / 1000;

  Serial.println();
  Serial.print("Content-Length: ");
  Serial.println(contentLength);
  Serial.print("Actually read:  ");
  Serial.println(readLength);
  Serial.print("Calc. CRC32:    0x");
  Serial.println(crc.finalize(), HEX);
  Serial.print("Known CRC32:    0x");
  Serial.println(knownCRC32, HEX);
  Serial.print("Duration:       ");
  Serial.print(duration);
  Serial.println("s");
  Serial.println("Wait for 3 seconds");

  for (int i = 0; i < 3; i++) {
    Serial.print(String(i) + "...");
    delay(1000);
  }

  //readFile(SPIFFS, "/update.bin");

  this->updateFromFS();

  // Do nothing forevermore
  while (true) {
    delay(1000);
  }
}

/**
 * @brief Mount the SPIFFS filesystem (this is in FLASH memory).
 * 
 * @return void
 */
bool GSMFirmwareUpdater::spiffsInit() // privte
{
  if (!SPIFFS.begin(false)) {
    Serial.println("[i] SPIFFS Mount Failed");
    return false;
  }
  
  Serial.println("[i] SPIFFS Mounted");
  SPIFFS.format();
  this->listDir(SPIFFS, "/", 0);
  return true;
}

void GSMFirmwareUpdater::writeUpdate(uint8_t *data, size_t len)
{
  size_t written = Update.write(data, len);
  Serial.print("Written: ");
  Serial.println(written);
  this->currentLength += len;
  Serial.print("Current Length: ");
  Serial.println(currentLength);
  Serial.print("Total Length: ");
  Serial.println(totalLength);
  // Print dots while waiting for update to finish
  Serial.print('.');
  // if current length of written firmware is not equal to total firmware size, repeat
  if (this->currentLength != this->totalLength)
    return;
  Update.end(true);
  Serial.printf("\nUpdate Success, Total Size: %u\nRebooting...\n", this->currentLength);
  // Restart ESP32 to see changes
  ESP.restart();
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

void GSMFirmwareUpdater::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
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

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void GSMFirmwareUpdater::updateFromFS()
{
  File updateBin = SPIFFS.open("/update.bin");
  if (updateBin)
  {
    if (updateBin.isDirectory()) {
      Serial.println("Error, in the directory");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      Serial.println("Trying to start update");
      this->beginProcessingUpdate(updateBin, updateSize);
    } else {
      Serial.println("Error, empty file");
    }

    updateBin.close();

    // whe finished remove the binary from sd card to indicate end of the process
    //fs.remove("/update.bin");
  } else {
    Serial.println("Can't upload file");
  }
}

void GSMFirmwareUpdater::beginProcessingUpdate(Stream &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      Serial.println("Written : " + String(written) + " successfully");
    } else {
      Serial.println("Only written : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }

    if (Update.end()) {
      Serial.println("OTA complete!");
      if (Update.isFinished()) {
        Serial.println("Ota successful, restarting!");
        ESP.restart();
      } else {
        Serial.println("Ota did not complete, something went wrong!");
      }
    } else {
      Serial.println("Error Occured #: " + String(Update.getError()));
    }
  } else {
    Serial.println("Not enough space to do OTA");
  }
}