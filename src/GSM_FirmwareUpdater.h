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
    uint16_t _error = 0;
    uint32_t _timeout = 260000;
    std::string _currentVersion = "";

    GSM_FirmwareUpdater(std::string currentVersion);
    ~GSM_FirmwareUpdater();

    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    bool checkUpdateAvailable(std::string availableVersionString);
    void updateFromFS(const char * updateFilePath);

private:
    std::string _availableVersion; // Firmware version available on the remote server
    int _respCode = 0; // HTTP response from GET requests

    bool spiffsInit();  
    void beginProcessingUpdate(Stream &updateSource, size_t updateSize);
    void writeUpdate(uint8_t *data, size_t len);
    std::string versionNumberFromString(std::string availableVersion);
};

#endif
