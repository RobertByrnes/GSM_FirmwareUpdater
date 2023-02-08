#define TINY_GSM_MODEM_SIM800
#define HTTPS_NO_GPRS_CONN          (0)
#define HTTPS_NONE_200_RESP         (1)

#include <string.h>
#include <TinyGSM.h>
#include <ArduinoHttpClient.h>
#include <CRC32.h>
#include "FS.h"
#include "SPIFFS.h"

class HTTPS {
    public:
    const static int _httpsTimeout = 30000L;
    static std::string get(TinyGsm &sim_modem, HttpClient &http_client, const char * resource);
    static std::string postJSON(TinyGsm &sim_modem, HttpClient &http_client, const char * endPoint, const char * requestBody, std::string currentToken);
    static std::string print(TinyGsm &sim_modem, HttpClient &http_client, const char * requestBody);
    static bool download(TinyGsm &sim_modem, HttpClient &http_client, const char * resource, const char * filePath, uint32_t knownCRC32=0);

    private:
    HTTPS();
};

/**
 * @brief Use the existing connection to send a GET request
 * 
 * @param sim_modem 
 * @param http_client 
 * @param resource 
 * 
 * @throws int HTTPS_NONE_200_RESP = 1
 */
std::string HTTPS::get(TinyGsm &sim_modem, HttpClient &http_client, const char * resource) {
    if (sim_modem.isGprsConnected()) {
        log_i("Making GET request");
        http_client.get(resource);
        int statusCode = http_client.responseStatusCode();
        log_i("Status code: %i", statusCode);
        if (statusCode == 200) {
            std::string responseBody = std::string(http_client.responseBody().c_str());
            log_i("Response: %s", responseBody.c_str());
            return responseBody;
        } else {
            throw HTTPS_NONE_200_RESP;
        }
    } else {
        throw HTTPS_NO_GPRS_CONN;
    }
}

std::string HTTPS::postJSON(TinyGsm &sim_modem, HttpClient &http_client, const char * endPoint, std::string requestBody, std::string currentToken) {
    http_client.setHttpResponseTimeout(HTTPS::_httpsTimeout);
    http_client.connectionKeepAlive();
    http_client.beginRequest();
    http_client.post(endPoint);
    http_client.sendHeader(HTTP_HEADER_CONTENT_TYPE, "application/json");
    http_client.sendHeader(HTTP_HEADER_CONTENT_LENGTH, requestBody.length());
    if (currentToken != "") {
        std::string authHeader = "Bearer " + currentToken;
        http_client.sendHeader("Authorization", authHeader.c_str());
    }
    http_client.endRequest();
    http_client.write((const byte*)requestBody.c_str(), requestBody.length());
    int status = http_client.responseStatusCode();
}

std::string HTTPS::print(TinyGsm &sim_modem, HttpClient &http_client, const char * requestBody) {
    if (sim_modem.isGprsConnected()) {
        log_i("Attempting to download");
        http_client.print(requestBody);
        int statusCode = http_client.responseStatusCode();
        log_i("Status code: %i", statusCode);
        if (statusCode == 200) {
            std::string response;
            std::string accessToken;
            unsigned long timeoutStart = millis();
            char c;

            while (
                (http_client.connected() || http_client.available()) &&
                (!http_client.endOfBodyReached()) &&
                ((millis() - timeoutStart) < HTTPS::_httpsTimeout)
            ) {
                c = http_client.read();
                response += c;
            }

            return response;
        } else {
            throw HTTPS_NONE_200_RESP;
        }
    } else {
        throw HTTPS_NO_GPRS_CONN;
    }    
}

bool HTTPS::download(TinyGsm &sim_modem, HttpClient &http_client, const char * resource, const char * filePath, uint32_t knownCRC32) {
    if (sim_modem.isGprsConnected()) {
        log_i("Attempting to download");
        http_client.get(resource);
        int statusCode = http_client.responseStatusCode();
        log_i("Status code: %i", statusCode);
        if (statusCode == 200) {
            
            if (SPIFFS.exists(filePath)) {
                SPIFFS.remove(filePath);
                log_i("Removed old %s", filePath);
            }

            int contentLength = http_client.contentLength();
            log_i("Content length: %i", contentLength);
            unsigned long timeoutStart = millis();
            int bin;
            // char c;
            CRC32 crc;
            uint8_t wbuf[512];
            uint32_t readLength = 0;
            File file = SPIFFS.open("/update.bin", "a");

            while (
                (http_client.connected() || http_client.available()) &&
                (!http_client.endOfBodyReached()) &&
                ((millis() - timeoutStart) < HTTPS::_httpsTimeout)
            ) {
                if (http_client.available()) {
                    bin = http_client.readBytes(wbuf, sizeof(wbuf));
                    readLength += file.write(wbuf, bin);
                    crc.update(bin);
                    Serial.print(bin);
                    // c = http_client.read(wbuf, sizeof(wbuf));
                    // readLength += file.write(wbuf, c);
                    // crc.update(c);
                    // Serial.print(c);
                    timeoutStart = millis();
                } else {
                    vTaskDelay(50);
                }
            }
            Serial.println();
            log_i(
                "\n%d bytes downloaded and writted to file : %s", 
                readLength, 
                file.name()
            );

            file.close();

            log_i("Actually read:  %u", readLength);
            log_i("Calc. CRC32:    0x%x", crc.finalize(), HEX);
            #if (knownCRC32 > 0)
            log_i("Known CRC32:    0x%x", _knownCRC32, HEX);
            #endif
            log_i("Duration:       %us", millis() - timeoutStart);
            log_i("Wait for 3 seconds");
            vTaskDelay(3000);

            if (contentLength == -1) {
                return true;
            } else if(readLength == contentLength) {
                return true;
            }

            return false;
        } else {
            throw HTTPS_NONE_200_RESP;
        }
    } else {
        throw HTTPS_NO_GPRS_CONN;
    }
}