#ifndef GSM_FIRMWARE_UPDATER_H
#define GSM_FIRMWARE_UPDATER_H

#include <CellularNetwork.h>
#include <OAuth2.h>
#include <CRC32.h>
#include <Update.h> 
#include "FS.h"
#include "SPIFFS.h"

#define GSM_FIRMWARE_UPDATER_NETWORK_ERROR              (1)
#define GSM_FIRMWARE_UPDATER_CLIENT_ERROR               (2)
#define GSM_FIRMWARE_UPDATER_CLIENT_TIMEOUT             (3)

class GSMFirmwareUpdater
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

    GSMFirmwareUpdater();
    ~GSMFirmwareUpdater();
    void setConfig(std::string &updateUrl, std::string &updateHost, uint16_t &port);
    void setTimeout(uint32_t timeout);
    void setCRC(uint32_t crc);
    bool spiffsInit();  
    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

    // template <typename ClientType, typename NetworkType>
    // void updateFirmware(ClientType &client, NetworkType &network);
    
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

protected:
    void updateFromFS();

private:
    void beginProcessingUpdate(Stream &updateSource, size_t updateSize);
    void writeUpdate(uint8_t *data, size_t len);

    // template <typename ClientType, typename NetworkType>
    // void openConnections(ClientType &client, NetworkType &network);
    
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

    // template <typename ClientType, typename NetworkType>
    // void shutdownConnections(ClientType &client, NetworkType &network);

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

    // template <typename ClientType>
    // int readFirmwareHeaders(ClientType &client);
    
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

    // template <typename ClientType, typename Len> 
    // int streamUpdateToFile(ClientType &client, Len contentLength);

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
