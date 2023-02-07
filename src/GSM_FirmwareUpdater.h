#ifndef GSM_FIRMWARE_UPDATER_H
#define GSM_FIRMWARE_UPDATER_H

#include <Update.h> 
#include "FS.h"
#include "SPIFFS.h"
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
    void beginProcessingUpdate(Stream &updateSource, size_t updateSize);
    std::string versionNumberFromString(std::string availableVersion);
};

#endif
