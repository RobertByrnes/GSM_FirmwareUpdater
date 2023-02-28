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
    void beginProcessingUpdate(T &updateSource, size_t updateSize);
    std::string versionNumberFromString(std::string availableVersion);
};

#endif
