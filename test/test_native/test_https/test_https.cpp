#include <emulation.h>
#include <https.hpp>
#include <modem.hpp>
#include <Mocks/MockTinyGsm.hpp>
#include <Mocks/MockSslClient.hpp>
#include <Mocks/MockHttpClient.hpp>

#define MODEM_UART Serial

const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
const char *hostName = "";
uint16_t port = 443;
uint16_t ledPin = 13;

MockTinyGsm modemDriverMock(MODEM_UART);
MockTinyGsmClient gsmTransportLayerMock(modemDriverMock);
MockSSLClient<MockTinyGsmClient> secureLayerMock(&gsmTransportLayerMock);
MockHttpClient httpClient = MockHttpClient(secureLayerMock, hostName, port);
Modem<MockTinyGsm> modemClass;
HTTPS<MockTinyGsm, MockHttpClient> https;

void setUp(void) {
    modemDriverMock.reset();
    httpClient.reset();
}

void tearDown(void) {}


void testGetMethodReturnsNonEmptyStringOnSuccess() {
    TEST_ASSERT_EQUAL_STRING("","");
}

void runTests() {
    // FILE *fp = freopen("output.txt", "a", stdout);
    UNITY_BEGIN();
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
    // fclose(fp);
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
