#ifndef GSM_FIRMWARE_UPDATER_H
#define GSM_FIRMWARE_UPDATER_H

#include <CellularNetwork.h>
#include <CRC32.h>
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
    int _totalLength = 0;
    int _currentLength = 0;
    std::string _updateUrl = "";
    std::string _updateHost = "";
    uint16_t _port = 0;
    uint16_t _error = 0;
    uint8_t _downloadAttempts = 5;
    uint32_t _knownCRC32 = 0;
    uint32_t _timeout = 260000;
    std::string _currentVersion = ""; // Current firmware version
    uint8_t _errorNumber = 0; // Error number

    GSM_FirmwareUpdater();
    ~GSM_FirmwareUpdater();
    void configure(const std::string &updateUrl, const std::string &updateHost, const uint16_t &port);
    void setTimeout(uint32_t timeout);
    void setCRC(uint32_t crc);
    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    std::string availableFirmwareVersion();

    /**
     * @brief use the WiFi class to return the local IP Address
     * 
     * @return IPAddress
     */
    template <typename NetworkType>
    IPAddress ipAddress(NetworkType &network) // public
    {
    return network._cellnet.localIP();
    }
    
    /**
     * @brief Function to update firmware incrementally.
     * Buffer is declared to be 128 so chunks of 128 bytes
     * from firmware is written to device until server closes.
     * 
     * @param http HttpClient
     * @param client TinyGsmClient
     * 
     * @return void
     */
    template <typename ClientType, typename NetworkType> 
    void updateFirmware(ClientType &client, NetworkType &network)
    { 
        int contentLength = 0;
        try { 
            this->openConnections(client, network);
            contentLength = this->readFirmwareHeaders(client);
        } catch (int error) {
            _error = error;
            return;
        }

        if (SPIFFS.exists("/update.bin")) {
            SPIFFS.remove("/update.bin");
            log_i("Removed old /update.bin");
        }

        log_i("Receiving response");

        this->streamUpdateToFile(client, contentLength);
        this->shutdownConnections(client, network);
        this->updateFromFS();
    }

    /**
     * @brief Connect to the update server and get the version file,
     * if the version is greater than the current version this 
     * function will return true, else it will be false.
     * 
     * @param versionFileUrl const char *
     * @return bool
     */
    template <typename ClientType, typename NetworkType> 
    bool checkUpdateAvailable(ClientType &client, NetworkType &network, const char * versionFileUrl)
    {
        int contentLength = 0;
        try { 
            this->openConnections(client, network);
            contentLength = this->readFirmwareHeaders(client);
        } catch (int error) {
            _error = error;
            return false;
        }

        if (_respCode == 200) {
            int len = _totalLength = client.getSize(); // get length of doc (is -1 when Server sends no Content-Length header)
            std::string currentVersion = this->versionNumberFromString(client, true);
            std::string availableVersion = this->versionNumberFromString(client, false);
            int check = Semver::versionCompare(currentVersion, availableVersion);

            if (check=-1) {
                return true;
            } else {
                return false;
            }
        } else {
            _errorNumber = 2;

            this->shutdownConnections(client, network);

            return false;
        }

        return true;
    }

    /**
     * @brief Parses the version number (semantic versioning) from a string read
     * from the GET request, or from the CURRENT_VERSION declared in 
     * version.h. Version is returned as an integer for comparison,
     * e.g. "version=1.2.8" will be returned as 128.
     * 
     * @param currentVersionCheck bool
     * @return int version or int error
     */
    template <typename ClientType>
    std::string versionNumberFromString(ClientType &client, bool currentVersionCheck)
    {
        std::string version;

        try {
            if (currentVersionCheck == false) {
                version = client.getString().c_str();
                version = version.substr(version.find_first_of("=") + 1);
                _availableVersion = version.c_str();

                if (_availableVersion == "") {
                    throw 5;
                }

                if (version == "") {
                    throw 6;
                } 
            } else {
                version = _currentVersion;
            }
        } catch (int error) {
            _errorNumber = error;
        }

        return version;
    }

protected:
    void updateFromFS();

private:
    std::string _availableVersion; // Firmware version available on the remote server
    int _respCode; // HTTP response from GET requests

    bool spiffsInit();  
    void beginProcessingUpdate(Stream &updateSource, size_t updateSize);
    void writeUpdate(uint8_t *data, size_t len);
    
    /**
     * @brief Make a connection to the mobile network and a HTTP connection to the firmware host.
     * 
     * @param TinyGsmClient &client
     * @param CellularNetwork &network
     * @throws GSM_FIRMWARE_UPDATER_NETWORK_ERROR int
     * @throws GSM_FIRMWARE_UPDATER_CLIENT_ERROR int
     * @return void
    */
    template <typename ClientType, typename NetworkType>
    void openConnections(ClientType &client, NetworkType &network)
    {
        if (!network.connectNetwork()) {
            log_e("Failed to connect to the mobile network");

            throw GSM_FIRMWARE_UPDATER_NETWORK_ERROR;
        }

        if (!client.connect(_updateHost.c_str(), _port)) {
            log_e(
            "Connecting to update server failed, Host: %s, Port: %u, Url: %s",
            _updateHost.c_str(),
            _port,
            _updateUrl.c_str()
            );

            throw GSM_FIRMWARE_UPDATER_CLIENT_ERROR;  
        }

        log_i("Connected to update server, requesting %s", _updateUrl.c_str());
    }

    /**
     * @brief Close connections to the update host and the mobile network.
     * 
     * @param &client TinyGsmClient
     * @param &network CellularNetwork
     * @return void 
    */
    template <typename ClientType, typename NetworkType>
    void shutdownConnections(ClientType &client, NetworkType &network)
    {
        client.stop();
        log_i("Server disconnected");

        network._cellnet.gprsDisconnect();
        log_i("GPRS disconnected");
    }
    
    /**
     * @brief Make HTTP GET request to firmware location and return the content length,
     * if successful the client will remain connected and ready to stream content.
     * 
     * @param TinyGsmClient &client
     * @throws GSM_FIRMWARE_UPDATER_CLIENT_TIMEOUT int
     * @return contentLength int
    */
    template <typename ClientType>
    int readFirmwareHeaders(ClientType &client)
    {
        uint32_t contentLength = 0;
        client.print(std::string("GET ").append(_updateUrl).append(" HTTP/1.0\r\n").c_str());
        client.print(std::string("Host: ").append(_updateHost).append("\r\n").c_str());
        client.print("Connection: close\r\n\r\n");
        log_i("Waiting for response header");

        long timeout = millis();

        while (client.available() == 0) {
            if (millis() - timeout > 20000L) {
            client.stop();
            delay(10000L);
            throw GSM_FIRMWARE_UPDATER_CLIENT_TIMEOUT;
            }
        }

        log_i("Header received");
        
        while (client.available()) {
            String line = client.readStringUntil('\n');
            line.trim();
            log_d("Reading line: %s", line.c_str());
            line.toLowerCase();
            if (line.startsWith("content-length:")) {
            contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
            log_i("Content length: %u", contentLength);
            } else if (line.length() == 0) {
            break;
            }
        }

        return contentLength;
    }

    /**
     * @brief Make a timed download of update file, the length of bytes read / written is compared,
     * to the content-length header sent by the update host.
     * 
     * @param TinyGsmClient &client
     * @param contentLength int
     * @return int
    */
    template <typename ClientType, typename Len>
    int streamUpdateToFile(ClientType &client, Len contentLength)
    {
        unsigned long timeElapsed = millis();
        uint32_t readLength = 0;
        CRC32 crc;
        uint8_t wbuf[256];
        File file = SPIFFS.open("/update.bin", FILE_APPEND);

        while (client.connected() && millis() - timeElapsed < _timeout) {
            while (client.available()) {
            int rd = client.readBytes(wbuf, sizeof(wbuf));
            readLength += file.write(wbuf, rd);
            crc.update(rd);
            }
        }

        log_i("%d bytes downloaded and writted to file : %s", readLength, file.name());

        file.close();

        log_i("Actually read:  %u", readLength);
        log_i("Calc. CRC32:    0x%x", crc.finalize(), HEX);
        log_i("Known CRC32:    0x%x", _knownCRC32, HEX);
        log_i("Duration:       %us", timeElapsed);
        log_i("Wait for 3 seconds");
        vTaskDelay(3000);

        if (readLength == contentLength) {
            return 1;
        }

        return 0;
    }
};


#endif
