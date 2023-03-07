#ifndef GSM_FIRMWARE_UPDATER_H
#define GSM_FIRMWARE_UPDATER_H

#if defined(ARDUINO)
#include <Update.h> 
#include "FS.h"
#include "SPIFFS.h"
#endif

#include <semver.hpp>

#define GSM_FIRMWARE_UPDATER_NETWORK_ERROR              (1)
#define GSM_FIRMWARE_UPDATER_CLIENT_ERROR               (2)
#define GSM_FIRMWARE_UPDATER_CLIENT_TIMEOUT             (3)

class GSM_FirmwareUpdater
{
    public:
    std::string _currentVersion = "";

    GSM_FirmwareUpdater(std::string currentVersion);
    ~GSM_FirmwareUpdater();

    bool checkUpdateAvailable(std::string availableVersionString);
    void updateFromFS(const char * updateFilePath);

    private:
    bool spiffsInit();
     
    template<typename T>
    void beginProcessingUpdate(T &updateSource, size_t updateSize)
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

    std::string versionNumberFromString(std::string availableVersion);
};

#endif
