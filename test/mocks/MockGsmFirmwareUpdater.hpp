#if not defined(MOCK_GSM_FIRMWARE_UPDATER_H)
#define MOCK_GSM_FIRMWARE_UPDATER_H

#include <Arduino.h>
#include <Emulator.h>
#include <SPIFFS.h>
#include "Update.h"
#include <FS.h>

class MockGsmFirmwareUpdater : public Emulator {
public:
	MockGsmFirmwareUpdater(std::string currentVersion) {}
	~MockGsmFirmwareUpdater() {}

    bool checkUpdateAvailable(std::string availableVersionString) { return this->mock<bool>("checkUpdateAvailable"); }
    void updateFromFS(const char * updateFilePath) {}

protected:

private:
    bool spiffsInit() {}

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

    std::string versionNumberFromString(std::string availableVersion) { return this->mock<std::string>("versionNumberFromString"); }
};

#endif
