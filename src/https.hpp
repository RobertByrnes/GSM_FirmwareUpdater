#define TINY_GSM_MODEM_SIM800
#define HTTPS_NO_GPRS_CONN                          (0)
#define HTTPS_NONE_200_RESP                         (1)
#define HTTPS_FAILED_HEADER_READ                    (2)
#define HTTPS_WRITE_BUFFER                          512
#define HTTPS_JSON_HEADER                           "application/json"
#define HTTPS_URLENCODED_HEADER                     "application/x-www-form-urlencoded"
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
#define HTTPS_METHOD_GET                            "GET"
#define HTTPS_METHOD_POST                           "POST"
#define HTTPS_METHOD_PUT                            "PUT"
#define HTTPS_METHOD_PATCH                          "PATCH"
#define HTTPS_METHOD_DELETE                         "DELETE"
#define HTTPS_HEADER_CONTENT_LENGTH                 "Content-Length"
#define HTTPS_HEADER_CONTENT_TYPE                   "Content-Type"
#define HTTPS_HEADER_ACCEPT                         "Accept"
#define HTTPS_HEADER_CONNECTION                     "Connection"
#define HTTPS_HEADER_TRANSFER_ENCODING              "Transfer-Encoding"
#define HTTPS_HEADER_USER_AGENT                     "User-Agent"
#define HTTPS_HEADER_VALUE_CHUNKED                  "chunked"

#include <string.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <CRC32.h>
#include "FS.h"
#include "SPIFFS.h"

using namespace std;

template <class ModemDriver, class HttpClientDriver>
class HTTPS {
    public:
    const static int _httpsTimeout = 60000L;
    HTTPS() {}
    ~HTTPS() {}
    
    /**
     * @brief Use the existing connection to send a GET request
     * 
     * @param simModem 
     * @param httpClient 
     * @param resource 
     * 
     * @throws int HTTPS_NONE_200_RESP = 1
     */
    string get(ModemDriver &simModem, HttpClientDriver &httpClient, const char * resource) {
        if (simModem.isGprsConnected()) {
            log_i("Making GET request");
            httpClient.get(resource);
            int statusCode = httpClient.responseStatusCode();
            log_i("Status code: %i", statusCode);
            if (statusCode == 200) {
                std::string responseBody = std::string(httpClient.responseBody().c_str());
                log_i("Response: %s", responseBody.c_str());
                return responseBody;
            } else {
                throw 1;
            }
        } else {
            throw 0;
        }
    }

    /**
     * @brief Send JSON in the POST request body, optionally the request
     * may be sent using OAUTH2 Authorization headers.
     * 
     * @param simModem 
     * @param httpClient 
     * @param endPoint 
     * @param requestBody 
     * @param currentToken 
     * @throws HTTPS_NONE_200_RESP
     * @return std::string 
     */
    string postJSON(
        ModemDriver &simModem, 
        HttpClientDriver &httpClient, 
        const char * endPoint, 
        string requestBody, 
        string currentToken=string("")
    ) {
        if (simModem.isGprsConnected()) {
            httpClient.setHttpResponseTimeout(HTTPS::_httpsTimeout);
            httpClient.connectionKeepAlive();
            httpClient.beginRequest();
            httpClient.post(endPoint);
            httpClient.sendHeader(HTTPS_HEADER_CONTENT_TYPE, HTTPS_JSON_HEADER);
            httpClient.sendHeader(HTTPS_HEADER_CONTENT_LENGTH, requestBody.length());
            if (!currentToken.empty()) {
                string authHeader = "Bearer " + currentToken;
                httpClient.sendHeader(HTTPS_OAUTH_HEADER, authHeader.c_str());
            }  
            httpClient.endRequest();
            httpClient.write((uint8_t*)requestBody.c_str(), requestBody.length());
            int statusCode = httpClient.responseStatusCode();
            log_i("Status code: %i", statusCode);
            if (this->responseOK(statusCode)) {
                this->readHeaders(httpClient);
                if (httpClient.isResponseChunked()) {
                    log_d("The response is chunked");
                }
                string responseBody = httpClient.responseBody().c_str();
                log_i("Response: %s", responseBody.c_str());
                log_d("Body length is: %i", responseBody.length());
                if (responseBody.length() > 0) {
                    return responseBody;
                } else {
                    return "";
                }
            }
            throw 1;
        } else {
            throw 0;
        } 
    }

    /**
     * @brief A simple API for sending a manually contructed request.
     * 
     * @param simModem 
     * @param httpClient 
     * @param endPoint 
     * @param requestBody 
     * @return std::string 
     * @throws HTTPS_NO_GPRS_CONN == 0
     * @throws HTTPS_NONE_200_RESP == 1
     */
    string print(ModemDriver &simModem, HttpClientDriver &httpClient, const char * endPoint, string requestBody) {
        if (simModem.isGprsConnected()) { 
            string response = "";
            httpClient.setHttpResponseTimeout(HTTPS::_httpsTimeout);
            httpClient.connectionKeepAlive();
            httpClient.beginRequest();
            httpClient.post(endPoint);
            httpClient.sendHeader(HTTPS_HEADER_ACCEPT, HTTPS_JSON_HEADER);
            httpClient.sendHeader(HTTPS_HEADER_CONTENT_TYPE, HTTPS_URLENCODED_HEADER);
            httpClient.sendHeader(HTTPS_HEADER_CONTENT_LENGTH, requestBody.length());
            httpClient.endRequest();
            httpClient.write((uint8_t*)requestBody.c_str(), requestBody.length());
            int statusCode = httpClient.responseStatusCode();
            log_i("Status code: %i", statusCode);
            if (this->responseOK(statusCode)) {
                unsigned long timeoutStart = millis();
                char c;
                while (
                    (httpClient.connected() || httpClient.available()) &&
                    (!httpClient.endOfBodyReached()) &&
                    ((millis() - timeoutStart) < this->_httpsTimeout)
                ) {
                    c = httpClient.read();
                    response += c;
                }

                return response;
            } else {
                log_e("Throwing None 200 response");
                throw 1;
            }
        } else { 
            log_e("Throwing GPRS disconnected");  
            throw 0;
        }    
    }

    /**
     * @brief Download a file using GET and save to the SPIFFS file system,
     * optionally the user may verify a known CRC32 hash of the file.
     * 
     * @param simModem 
     * @param httpClient 
     * @param resource 
     * @param filePath 
     * @param knownCRC32 
     * @return true 
     * @return false 
     * @throws HTTPS_NONE_200_RESP
     * @throws HTTPS_NO_GPRS_CONN
     */
    template<typename SPIFFSType>
    bool download(
        ModemDriver &simModem, 
        HttpClientDriver &httpClient, 
        SPIFFSType &Spiffs,
        const char * resource, 
        const char * filePath, 
        uint32_t knownCRC32=0
    ) {
        if (simModem.isGprsConnected()) {
            log_i("Attempting to download");
            httpClient.get(resource);
            int statusCode = httpClient.responseStatusCode();
            log_i("Status code: %i", statusCode);
            if (this->responseOK(statusCode)) {
                
                if (Spiffs.exists(filePath)) {
                    Spiffs.remove(filePath);
                    log_i("Removed old %s", filePath);
                }

                int contentLength = httpClient.contentLength();
                log_i("Content length: %i", contentLength);
                int bin;
                CRC32 crc;
                uint8_t wbuf[HTTPS_WRITE_BUFFER];
                uint32_t readLength = 0;
                File file = SPIFFS.open(filePath, "a");
                unsigned long timeoutStart = millis();
                while (
                    (httpClient.connected() || httpClient.available()) &&
                    (!httpClient.endOfBodyReached()) 
                    // && ((millis() - timeoutStart) < this->_httpsTimeout)
                ) {
                    if (httpClient.available()) {
                        bin = httpClient.readBytes(wbuf, sizeof(wbuf));
                        readLength += file.write(wbuf, bin);
                        crc.update(bin);
                        timeoutStart = millis();
                    } else {
                        vTaskDelay(50);
                    }
                }
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
                throw 1;
            }
        } else {
            throw 0;
        }
    }

    /**
     * @brief Read the HTTP response headers to the output stream,
     * errors if times out or 
     * 
     * @param httpClient 
     */
    template<class HttpDriver>
    void readHeaders(HttpDriver &httpClient) {
        string headerName, headerValue;
        try {
            unsigned long headerTimeout = millis();
            while (httpClient.headerAvailable() && millis() - headerTimeout < this->_httpsTimeout) {
                headerName = httpClient.readHeaderName().c_str();
                headerValue = httpClient.readHeaderValue().c_str();
                log_i("%s : %s", headerName.c_str(), headerValue.c_str());
            }
            int length = httpClient.contentLength();
            if (length >= 0) {
                log_i("Content length: %i", length);
            }
        } catch(uint8_t error) {
            throw 2;
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
    bool responseOK(int statusCode) {
        switch (statusCode) {
            case HTTPS_OK_RESPONSE_CODE:
            case HTTPS_ACCEPTED_RESPONSE_CODE:
            case HTTPS_CREATED_RESPONSE_CODE: return true; break;
            default: return false;
        }
    }
};
