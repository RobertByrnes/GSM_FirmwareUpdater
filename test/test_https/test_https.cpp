#include <unity.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <modem.hpp>
#include <mock_tiny_gsm.cpp>
#include <mock_ssl_client.cpp>
#include <mock_http_client.cpp>

#define MODEM_UART Serial1

const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
const char *hostName = "";
uint16_t port = 443;
uint16_t ledPin = 13;

MockTinyGsm modemDriverMock(MODEM_UART);
MockTinyGsmClient gsmTransportLayerMock(modemDriverMock);
MockSSLClient secureLayerMock(&gsmTransportLayerMock);
MockHttpClient httpClient = MockHttpClient(secureLayerMock, hostName, port);
Modem<MockTinyGsm> modemClass;

void setUp() {}
void tearDown() {}

void setup() {
    UNITY_BEGIN();
    UNITY_END();
}
void loop() {}
