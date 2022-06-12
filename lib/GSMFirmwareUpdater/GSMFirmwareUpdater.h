#ifndef GSM_FIRMWARE_UPDATER_H
#define GSM_FIRMWARE_UPDATER_H

#include <CellularNetwork800L.h>
#include <OAuth2.h>
#include <CRC32.h>
#include <Update.h>
#include "FS.h"
#include "SPIFFS.h"

class GSMFirmwareUpdater
{
public:
    int totalLength;
    int currentLength;
    const char *updateUrl;
    const char *updateHost;
    const uint16_t port;

    GSMFirmwareUpdater(const char *UPDATE_URL, const char *UPDATE_HOST, const uint16_t &PORT);
    ~GSMFirmwareUpdater();

    bool spiffsInit(); 
    void updateFirmware(TinyGsmClientSecure &client, CellularNetwork800L &network);
    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

private:
    bool connectNetowrk(CellularNetwork800L &network);
    void beginProcessingUpdate(Stream &updateSource, size_t updateSize);
    void printPercent(uint32_t readLength, uint32_t contentLength);
    const char *cleanUrl(const char *url);
    void writeUpdate(uint8_t *data, size_t len);
    void updateFromFS();
};


#endif