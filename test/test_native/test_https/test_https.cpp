#include <unity.h>

#if not defined(ARDUINO)
#include <ArduinoFake.h>
#define log_i(...) 
#define log_w(...)
#define log_d(...) 
#define log_e(...) 
#endif

#include <modem.hpp>
#include <mock_tiny_gsm.hpp>
#include <mock_ssl_client.cpp>
#include <mock_http_client.cpp>

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

void setUp(void) {
    modemDriverMock.reset();
    httpClient.reset();
}

void tearDown(void) {}


void runTests() {
    // FILE *fp = freopen("output.txt", "a", stdout);
    UNITY_BEGIN();
    //
    UNITY_END();
    // fclose(fp);
}

#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
    runTests();
}

void loop() {}

#else

int main(int argc, char **argv)
{
    runTests();
    return 0;
}

#endif
