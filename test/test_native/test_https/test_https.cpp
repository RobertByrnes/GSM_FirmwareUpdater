// #define EMULATOR_LOG

#include <emulation.h>
#include <https.hpp>
#include <modem.hpp>
#include <TinyGsm.h>
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

void setUp(void) {
    modemDriverMock.reset();
    mockHttpClient.reset();
}

void tearDown(void) {}

void testGetMethodReturnsNonEmptyStringOnSuccess() {
    String successReponse = "Success was received";
    modemDriverMock.returns("isGprsConnected", true);
    mockHttpClient.returns("get", 0);
    mockHttpClient.returns("responseStatusCode", 200);
    mockHttpClient.returns("responseBody", successReponse);
    std::string response = https.get(modemDriverMock, mockHttpClient, fakeResource);
    TEST_ASSERT_EQUAL_STRING(successReponse.c_str(), response.c_str());
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(testGetMethodReturnsNonEmptyStringOnSuccess);
    // TODO add testGetMethodThrowsOnNone200Response();
    // TODO add testGetMethodThrowsIfModemNotConnected();
    // TODO add testPostJSONMethodReturnsNonEmptyStringOnSuccess(); TEST_ASSERT_EQUAL_STRING
    // TODO add testPostJsonMethodReturnsEmptyStringIfResponseBodyLengthZero(); TEST_ASSERT_EQUAL_STRING
    // TODO add testPostJsonMethodThrowsOnNone200Response(); // consider adding 201 to check - see responseOK function
    // TODO add testPrintMethodReturnsNonEmptyStringOnSuccess(); TEST_ASSERT_EQUAL_STRING
    // TODO add testPrintMethodThrowsOnNone200Response();
    // TODO add testPrintMethodThrowsIfModemNotConnected();
    // TODO add testResponseOKMethodReturnsTrueIfStatusCodeInSwitch();
    // TODO add testResponseOKMethodReturnsFalseIfStatusCodeNotInSwitch();
    // TODO add testDownloadMethodReturnsTrueIfDownloadSucceedsWithNoContentLengthHeader(); TEST_ASSERT_TRUE
    // TODO add testDownloadMethodReturnsTrueIfDownloadSucceedsWithContentLengthHeader(); TEST_ASSERT_TRUE 
    // TODO add testDownloadMethodReturnsFalseIfContentLengthCheckNotSatisfied(); TEST_ASSERT_FALSE
    // TODO add testDownloadMethodThrowsOnNone200Response();
    // TODO add testDownloadMethodThrowsIfModemNotConnected();
    // TODO add testReadHeadersDoesNotThrowIfHeadersValid();
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
