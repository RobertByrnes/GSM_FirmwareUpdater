#define TINY_GSM_MODEM_SIM800
#define HTTPS_NO_GPRS_CONN                          (0)
#define HTTPS_NONE_200_RESP                         (1)
#define HTTPS_FAILED_HEADER_READ                    (2)
#define HTTPS_WRITE_BUFFER                          512
#define HTTPS_JSON_HEADER                           "application/json"
#define HTTPS_OAUTH_HEADER                          "Authorization"
#define HTTPS_OK_RESPONSE_CODE                      200
#define HTTPS_CREATED_RESPONSE_CODE                 201
#define HTTPS_ACCEPTED_RESPONSE_CODE                202
// #define HTTPS_MOVED_RESPONSE_CODE                   301
// #define HTTPS_FOUND_RESPONSE_CODE                   302
// #define HTTPS_BAD_REQUEST_RESPONSE_CODE             401
// #define HTTPS_UNAUTHORIZED_RESPONSE_CODE            401
// #define HTTPS_FORBIDDEN_RESPONSE_CODE               403
// #define HTTPS_NOT_FOUND_RESPONSE_CODE               404
// #define HTTPS_UNPROCESSIBLE_RESPONSE_CODE           422
// #define HTTPS_SERVER_ERROR_RESPONSE_CODE            500
// #define HTTPS_BAD_GATEWAY_RESPONSE_CODE             502
// #define HTTPS_SERVICE_UNAVAILABLE_RESPONSE_CODE     503
// #define HTTPS_GATEWAY_TIMEOUT_RESPONSE_CODE         504


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
    static std::string postJSON(TinyGsm &sim_modem, HttpClient &http_client, const char * endPoint, std::string requestBody, std::string currentToken="");
    static std::string print(TinyGsm &sim_modem, HttpClient &http_client, const char * requestBody);
    static bool download(TinyGsm &sim_modem, HttpClient &http_client, const char * resource, const char * filePath, uint32_t knownCRC32=0);
    static void readHeaders(HttpClient &http_client);

    private:
    HTTPS();
    static bool HTTPS::responseOK(int statusCode);
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

/**
 * @brief Send JSON in the POST request body, optionally the request
 * may be sent using OAUTH2 Authorization headers.
 * 
 * @param sim_modem 
 * @param http_client 
 * @param endPoint 
 * @param requestBody 
 * @param currentToken 
 * @throws HTTPS_NONE_200_RESP
 * @return std::string 
 */
std::string HTTPS::postJSON(
    TinyGsm &sim_modem, 
    HttpClient &http_client, 
    const char * endPoint, 
    std::string requestBody, 
    std::string currentToken
) {
    http_client.setHttpResponseTimeout(HTTPS::_httpsTimeout);
    http_client.connectionKeepAlive();
    http_client.beginRequest();
    http_client.post(endPoint);
    http_client.sendHeader(HTTP_HEADER_CONTENT_TYPE, HTTPS_JSON_HEADER);
    http_client.sendHeader(HTTP_HEADER_CONTENT_LENGTH, requestBody.length());
    if (currentToken != "") {
        std::string authHeader = "Bearer " + currentToken;
        http_client.sendHeader(HTTPS_OAUTH_HEADER, authHeader.c_str());
    }
    http_client.endRequest();
    http_client.write((const byte*)requestBody.c_str(), requestBody.length());
    int statusCode = http_client.responseStatusCode();
    log_i("Status code: %i", statusCode);

    if (HTTPS::responseOK(statusCode)) {
        HTTPS::readHeaders(http_client);
        if (http_client.isResponseChunked()) {
            log_d("The response is chunked");
        }
        std::string responseBody = http_client.responseBody().c_str();
        log_i("Response: %s", responseBody.c_str());
        log_d("Body length is: %i", responseBody.length());
        if (responseBody.length() > 0) {
            return responseBody;
        } else {
            return "";
        }
    }
    throw HTTPS_NONE_200_RESP;
}

/**
 * @brief A simple API for sending a manually contructed request.
 * The request
 * 
 * @param sim_modem 
 * @param http_client 
 * @param requestBody 
 * @return std::string 
 * @throws HTTPS_NO_GPRS_CONN == 0
 * @throws HTTPS_NONE_200_RESP == 1
 */
std::string HTTPS::print(TinyGsm &sim_modem, HttpClient &http_client, const char * requestBody) {
    if (sim_modem.isGprsConnected()) {
        log_i("Attempting to download");
        http_client.print(requestBody);
        int statusCode = http_client.responseStatusCode();
        log_i("Status code: %i", statusCode);
        if (HTTPS::responseOK(statusCode)) {
            std::string response;
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

/**
 * @brief Download a file using GET and save to the SPIFFS file system,
 * optionally the user may verify a known CRC32 hash of the file.
 * 
 * @param sim_modem 
 * @param http_client 
 * @param resource 
 * @param filePath 
 * @param knownCRC32 
 * @return true 
 * @return false 
 * @throws HTTPS_NONE_200_RESP
 * @throws HTTPS_NO_GPRS_CONN
 */
bool HTTPS::download(
    TinyGsm &sim_modem, 
    HttpClient &http_client, 
    const char * resource, 
    const char * filePath, 
    uint32_t knownCRC32
) {
    if (sim_modem.isGprsConnected()) {
        log_i("Attempting to download");
        http_client.get(resource);
        int statusCode = http_client.responseStatusCode();
        log_i("Status code: %i", statusCode);
        if (HTTPS::responseOK(statusCode)) {
            
            if (SPIFFS.exists(filePath)) {
                SPIFFS.remove(filePath);
                log_i("Removed old %s", filePath);
            }

            int contentLength = http_client.contentLength();
            log_i("Content length: %i", contentLength);
            unsigned long timeoutStart = millis();
            int bin;
            CRC32 crc;
            uint8_t wbuf[HTTPS_WRITE_BUFFER];
            uint32_t readLength = 0;
            File file = SPIFFS.open(filePath, "a");

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

/**
 * @brief Read the HTTP response headers to the output stream,
 * errors if times out or 
 * 
 * @param http_client 
 */
void HTTPS::readHeaders(HttpClient &http_client) {
    std::string headerName, headerValue;
    try {
        unsigned long headerTimeout = millis();
        while (http_client.headerAvailable() && millis() - headerTimeout < HTTPS::_httpsTimeout) {
            headerName = http_client.readHeaderName().c_str();
            headerValue = http_client.readHeaderValue().c_str();
            log_i("%s : %s", headerName.c_str(), headerValue.c_str());
        }

        int length = http_client.contentLength();

        if (length >= 0) {
            log_i("Content length: %i", length);
        }
    } catch(uint8_t error) {
        throw HTTPS_FAILED_HEADER_READ;
        log_e("failed when reading response headers, last header read: '%s': '%s'", headerName.c_str(), headerValue.c_str());
    }
}

/**
 * @brief Verifiy the response code is in the accepted group of codes
 * 
 * @param statusCode 
 * @return true 
 * @return false 
 */
bool HTTPS::responseOK(int statusCode) {
    switch (statusCode) {
        case HTTPS_OK_RESPONSE_CODE:
        case HTTPS_ACCEPTED_RESPONSE_CODE:
        case HTTPS_CREATED_RESPONSE_CODE: return true; break;
        default: return false;
    }
}