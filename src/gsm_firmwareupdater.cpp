#include "GSMFirmwareUpdater.h"

// Constructor
GSMFirmwareUpdater::GSMFirmwareUpdater() {}

// Destructor
GSMFirmwareUpdater::~GSMFirmwareUpdater() {}

/**
 * @brief Set parameters for the update host.
 * 
 * @param updateUrl &std::string
 * @param updateHost &std::string
 * @param port &uint16_t
 * @return void
*/
void GSMFirmwareUpdater::setConfig(std::string &updateUrl, std::string &updateHost, uint16_t &port)
{
  _updateUrl = updateUrl;
  _updateHost = updateHost;
  _port = port;
}

/**
 * @brief Set the known CRC32 hash of the update file for use in comparison with the
 * downloaded update file.
 * 
 * @param uint32_t crc
 * @return void 
*/
void GSMFirmwareUpdater::setCRC(uint32_t crc)
{
  _knownCRC32 = crc;
}

/**
 * @brief Mount the SPIFFS filesystem (this is in FLASH memory).
 * 
 * @return void
 */
bool GSMFirmwareUpdater::spiffsInit()
{
  if (!SPIFFS.begin(false)) {
    log_e("SPIFFS Mount Failed");
    return false;
  }
  
  log_i("SPIFFS Mounted");
  return true;
}

void GSMFirmwareUpdater::updateFromFS()
{
  File updateBin = SPIFFS.open("/update.bin");
  if (updateBin) {
    if (updateBin.isDirectory()) {
      log_e("Error, in the directory");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      log_i("Trying to start update");
      this->beginProcessingUpdate(updateBin, updateSize);
    } else {
      log_e("Error, empty file");
    }

    updateBin.close();

    // whe finished remove the binary from sd card to indicate end of the process
    SPIFFS.remove("/update.bin");
  } else {
    log_e("Can't open update file");
  }
}

void GSMFirmwareUpdater::setTimeout(uint32_t timeout)
{
  _timeout = timeout;
}

void GSMFirmwareUpdater::beginProcessingUpdate(Stream &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      log_i("Written: %i successfully", written);
    } else {
      log_i("Only written: %i / %i. Will Retry...", written, updateSize);
    }

    if (Update.end()) {
      log_i("OTA complete");
      if (Update.isFinished()) {
        SPIFFS.remove("/update.bin");
        log_i("Ota successful, restarting");
        ESP.restart();
      } else {
        log_e("Ota did not complete, something went wrong");
      }
    } else {
      log_e("Error Occured #: %u", Update.getError());
    }
  } else {
    log_e("Not enough space to do OTA");
  }
}