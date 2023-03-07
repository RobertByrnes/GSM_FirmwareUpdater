#define EMULATOR_LOG
#ifndef TINY_GSM_MODEM_SIM800
#define TINY_GSM_MODEM_SIM800
#endif
#include <emulation.h>
#include <MethodLog.h>
#include <https.hpp>
#include <modem.hpp>
#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <ArduinoHttpClient.h>
#include <CRC32.h>
#include <SPIFFS.h>

#define MODEM_UART Serial

const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
const char *hostName = "";
uint16_t port = 443;
uint16_t ledPin = 13;
const char * fakeResource = "/path/to_resource";

TinyGsm modemDriverMock(MODEM_UART);
TinyGsmClient gsmTransportLayerMock(modemDriverMock);
SSLClient<TinyGsmClient> secureLayerMock(&gsmTransportLayerMock);
HttpClient mockHttpClient = HttpClient(secureLayerMock, hostName, port);
Modem<TinyGsm> modemClass;
HTTPS<TinyGsm, HttpClient> https;
fs::SPIFFSFS SPIFFS(impl);
MethodLog methodLogger;

String successReponse = "Develop success from failures.";
std::string requestBody = "The road to success and the road to failure are almost exactly the same.";

void setUp(void) {
    modemDriverMock.reset();
    mockHttpClient.reset();
}

void tearDown(void) {}

void testGetMethodReturnsNonEmptyStringOnSuccess() {
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("get", 0);
    mockHttpClient.returns("responseStatusCode", 200);
    mockHttpClient.returns("responseBody", successReponse);
    std::string response = https.get(modemDriverMock, mockHttpClient, fakeResource);
    TEST_ASSERT_EQUAL_STRING(successReponse.c_str(), response.c_str());
}

void testGetMethodThrowsOnNone200Response() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", true);
        mockHttpClient.returns("get", 0);
        mockHttpClient.returns("responseStatusCode", 403);
        std::string response = https.get(modemDriverMock, mockHttpClient, fakeResource);
    } catch (int exception) {
        if (exception == 1) { exceptionThrown = true; }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testGetMethodThrowsIfModemNotConnected() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", false);
        std::string response = https.get(modemDriverMock, mockHttpClient, fakeResource);
    } catch (int exception) {
        if (exception == 0) { exceptionThrown = true; }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testPostJSONMethodReturnsNonEmptyStringOnSuccess() {
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("post", 0);
    mockHttpClient.returns("write", (size_t)1024);
    mockHttpClient.returns("responseStatusCode", 200);
    mockHttpClient.returns("isResponseChuncked", 0);
    mockHttpClient.returns("headerAvailable", true).then(false);
    mockHttpClient.returns("readHeaderName", String("header"));
    mockHttpClient.returns("readHeaderValue", String("value"));
    mockHttpClient.returns("contentLength", 1000);
    mockHttpClient.returns("responseBody", successReponse);
    std::string response = https.postJSON(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    TEST_ASSERT_EQUAL_STRING(successReponse.c_str(), response.c_str());
    methodLogger.dumpMethodProfiles(mockHttpClient._methods);
}

void testPostJsonMethodReturnsEmptyStringIfResponseBodyLengthZero() {
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("post", 0);
    mockHttpClient.returns("write", (size_t)1024);
    mockHttpClient.returns("responseStatusCode", 200);
    mockHttpClient.returns("isResponseChuncked", 0);
    mockHttpClient.returns("headerAvailable", true).then(false);
    mockHttpClient.returns("readHeaderName", String("header"));
    mockHttpClient.returns("readHeaderValue", String("value"));
    mockHttpClient.returns("contentLength", 0);
    mockHttpClient.returns("responseBody", String(""));
    std::string response = https.postJSON(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    TEST_ASSERT_EQUAL_STRING(String("").c_str(), response.c_str());
}

void testPostJsonMethodThrowsOnNone200Response() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", true);
        mockHttpClient.returns("post", 0);
        mockHttpClient.returns("write", (size_t)1024);
        mockHttpClient.returns("responseStatusCode", 500);
        std::string response = https.postJSON(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    } catch (int exception) {
        if (exception == 1) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testPrintMethodReturnsNonEmptyStringOnSuccess() {
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("print", (size_t)1024);
    mockHttpClient.returns("responseStatusCode", 200);
    mockHttpClient.returns("connected", (uint8_t)1);
    mockHttpClient.returns("available", 1024);
    mockHttpClient.returns("endOfBodyReached", false).then(true);
    mockHttpClient.returns("read", 83);
    std::string response = https.print(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    TEST_ASSERT_EQUAL_STRING("S", response.c_str());
}

void testPrintMethodThrowsOnNone200Response() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", true);
        mockHttpClient.returns("print", (size_t)1024);
        mockHttpClient.returns("responseStatusCode", 500);
        std::string response = https.print(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    } catch (int exception) {
        if (exception == 1) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testPrintMethodThrowsIfModemNotConnected() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", false);
        std::string response = https.print(modemDriverMock, mockHttpClient, fakeResource, requestBody);
    } catch (int exception) {
        if (exception == 0) { exceptionThrown = true; }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testResponseOKMethodReturnsTrueIfStatusCodeInSwitch() {
    TEST_ASSERT_TRUE(https.responseOK(200));
}

void testResponseOKMethodReturnsFalseIfStatusCodeNotInSwitch() {
    TEST_ASSERT_FALSE(https.responseOK(502));
}

void testReadHeadersDoesNotThrowIfHeadersValid() {
    bool exceptionThrown = false;
    try {
        mockHttpClient.returns("headerAvailable", true).then(false);
        mockHttpClient.returns("readHeaderName", String("Content-Length"));
        mockHttpClient.returns("readHeaderValue", String("1000"));
        mockHttpClient.returns("contentLength", 1000);
        https.readHeaders(mockHttpClient);
    } catch (int exception) {
        if (exception == 2) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void testReadHeadersThrowsIfHeadersInvalid() {
    bool exceptionThrown = false;
    try {
        mockHttpClient.setException("headerAvailable", 2);
        mockHttpClient.returns("headerAvailable", false);
        mockHttpClient.returns("contentLength", 1000);
        https.readHeaders(mockHttpClient);
    } catch (int exception) {
        if (exception == 2) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testDownloadMethodReturnsTrueIfDownloadSucceedsWithNoContentLengthHeader() {
    _milliseconds = 1000;
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("get", 0);
    mockHttpClient.returns("responseStatusCode", 200);
    SPIFFS.returns("exists", true);
    SPIFFS.returns("remove", true);
    mockHttpClient.returns("contentLength", -1);
    SPIFFS.returns("open", true);
    mockHttpClient.returns("connected", (uint8_t)1).then((uint8_t)0);
    mockHttpClient.returns("available", 0).then(256);
    mockHttpClient.returns("endOfBodyReached", false).then(true);
    mockHttpClient.returns("readBytes", 100);
    File file;
    file.returns("write", 1);
    TEST_ASSERT_TRUE(https.download(modemDriverMock, mockHttpClient, SPIFFS, fakeResource, fakeResource));
}

void testDownloadMethodReturnsTrueIfDownloadSucceedsWithContentLengthHeader() {
    _milliseconds = 1000;
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("get", 0);
    mockHttpClient.returns("responseStatusCode", 200);
    SPIFFS.returns("exists", true);
    SPIFFS.returns("remove", true);
    mockHttpClient.returns("contentLength", 54);
    SPIFFS.returns("open", true);
    mockHttpClient.returns("connected", (uint8_t)1).then((uint8_t)0);
    mockHttpClient.returns("available", 54);
    mockHttpClient.returns("endOfBodyReached", false).then(true);
    mockHttpClient.returns("readBytes", 54);
    File file;
    file.returns("write", 1);
    TEST_ASSERT_TRUE(https.download(modemDriverMock, mockHttpClient, SPIFFS, fakeResource, fakeResource));
    methodLogger.dumpMethodProfiles(mockHttpClient._methods);
}

void testDownloadMethodReturnsFalseIfContentLengthCheckNotSatisfied() {
    _milliseconds = 1000;
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("get", 0);
    mockHttpClient.returns("responseStatusCode", 200);
    SPIFFS.returns("exists", true);
    SPIFFS.returns("remove", true);
    mockHttpClient.returns("contentLength", 540);
    SPIFFS.returns("open", true);
    mockHttpClient.returns("connected", (uint8_t)1).then((uint8_t)0);
    mockHttpClient.returns("available", 54);
    mockHttpClient.returns("endOfBodyReached", false).then(true);
    mockHttpClient.returns("readBytes", 54);
    File file;
    file.returns("write", 1);
    TEST_ASSERT_FALSE(https.download(modemDriverMock, mockHttpClient, SPIFFS, fakeResource, fakeResource));
    methodLogger.dumpMethodProfiles(mockHttpClient._methods);
}

void testDownloadMethodThrowsOnNone200Response() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", true);
        mockHttpClient.returns("get", 0);
        mockHttpClient.returns("responseStatusCode", 400);
        https.download(modemDriverMock, mockHttpClient, SPIFFS, fakeResource, fakeResource);
    } catch (int exception) {
        if (exception == 1) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testDownloadMethodThrowsIfModemNotConnected() {
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("isGprsConnected", false);
        https.download(modemDriverMock, mockHttpClient, SPIFFS, fakeResource, fakeResource);
    } catch (int exception) {
        if (exception == 0) {
            exceptionThrown = true;
        }
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(testGetMethodReturnsNonEmptyStringOnSuccess);
    RUN_TEST(testGetMethodThrowsOnNone200Response);
    RUN_TEST(testGetMethodThrowsIfModemNotConnected);
    RUN_TEST(testPostJSONMethodReturnsNonEmptyStringOnSuccess);
    RUN_TEST(testPostJsonMethodReturnsEmptyStringIfResponseBodyLengthZero);
    RUN_TEST(testPostJsonMethodThrowsOnNone200Response); // consider adding 201 to check - see responseOK function
    // RUN_TEST(testPrintMethodReturnsNonEmptyStringOnSuccess);
    RUN_TEST(testPrintMethodThrowsOnNone200Response);
    RUN_TEST(testPrintMethodThrowsIfModemNotConnected);
    RUN_TEST(testResponseOKMethodReturnsTrueIfStatusCodeInSwitch);
    RUN_TEST(testResponseOKMethodReturnsFalseIfStatusCodeNotInSwitch);
    RUN_TEST(testDownloadMethodReturnsTrueIfDownloadSucceedsWithNoContentLengthHeader);
    // RUN_TEST(testDownloadMethodReturnsTrueIfDownloadSucceedsWithContentLengthHeader); 
    RUN_TEST(testDownloadMethodReturnsFalseIfContentLengthCheckNotSatisfied);
    RUN_TEST(testDownloadMethodThrowsOnNone200Response);
    RUN_TEST(testDownloadMethodThrowsIfModemNotConnected);
    RUN_TEST(testReadHeadersDoesNotThrowIfHeadersValid);
    RUN_TEST(testReadHeadersThrowsIfHeadersInvalid);
    UNITY_END();
}

#if defined(ARDUINO)
#include <Arduino.h>

void setup() {
    runTests();
}

void loop() {}

#else

int main(int argc, char **argv) {
    runTests();
    return 0;
}

#endif
