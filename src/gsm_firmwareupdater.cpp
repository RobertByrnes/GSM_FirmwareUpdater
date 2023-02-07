#include "GSM_FirmwareUpdater.h"

// Constructor
GSM_FirmwareUpdater::GSM_FirmwareUpdater(std::string currentVersion): _currentVersion(currentVersion)
{
  this->spiffsInit();
}

// Destructor
GSM_FirmwareUpdater::~GSM_FirmwareUpdater() {}


/**
 * @brief Mount the SPIFFS filesystem (this is in FLASH memory).
 * 
 * @return void
 */
bool GSM_FirmwareUpdater::spiffsInit()
{
  if (!SPIFFS.begin(false)) {
    log_e("SPIFFS Mount Failed");
    return false;
  }
  
  log_i("SPIFFS Mounted");
  return true;
}

void GSM_FirmwareUpdater::updateFromFS(const char * updateFilePath)
{
  File updateBin = SPIFFS.open(updateFilePath);
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

void GSM_FirmwareUpdater::beginProcessingUpdate(Stream &updateSource, size_t updateSize)
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

  /**
   * @brief Connect to the update server and get the version file,
   * if the version is greater than the current version this 
   * function will return true, else it will be false.
   * 
   * @param availableVersionString std::string
   * 
   * @return bool
   */
  bool GSM_FirmwareUpdater::checkUpdateAvailable(std::string availableVersionString)
  { 
      std::string availableVersion = this->versionNumberFromString(availableVersionString);
      log_i("Available Version: %s", availableVersion.c_str());
      log_i("Current Version: %s", _currentVersion.c_str());
      int check = Semver::versionCompare(_currentVersion, availableVersion);
      log_i("Check result: %i", check);
      
      if (check == -1) {
          return true;
      } else {
          return false;
      }
  }

    /**
     * @brief Parses the version number from a string read of the format
     * "version=1.2.8". Version is returned as an integer for comparison,
     * e.g. "version=1.2.8" will be returned as 128.
     * 
     * @param availableVersion std::string
     * 
     * @return int version or int error
     */
    std::string GSM_FirmwareUpdater::versionNumberFromString(std::string availableVersion)
    {
        std::string version;
        version = availableVersion.substr(availableVersion.find_first_of("=") + 1);

        if (version == "") {
            throw 6;
        } 

        return version;
    }
