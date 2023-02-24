#if not defined(ARDUINO)
#include <ArduinoFake.h>
#define log_i(...)
#define log_w(...)
#define log_d(...) 
#define log_e(...) 
#endif

#include <iostream>
#include <unity.h>
#include <vector>
#include <any>
#include <PsuedoType.hpp>
#include <modem.hpp>
#include <mock_tiny_gsm.hpp>

#define MODEM_UART Serial
MockTinyGsm modemDriverMock(MODEM_UART);
Modem<MockTinyGsm> modemClass;
const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
uint16_t ledPin = 13;

void setUp() {}
void tearDown() {}

void testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork()
{
    bool exceptionThrown = false;
    int exception;
    try {    
        modemDriverMock.returns("waitForNetwork", true);
        modemClass.awaitNetworkAvailability(modemDriverMock, 3000L);
    } catch (int exception) {
        exceptionThrown = true;
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void testAwaitNetworkAvailabilityThrowsIfModemFailsToFindANetwork()
{
    bool exceptionThrown = false;
    int exception;
    try {    
        modemDriverMock.returns("waitForNetwork", true);
        modemDriverMock.setException("waitForNetwork", MODEM_NO_NETWORK_CONN);
        modemClass.awaitNetworkAvailability(modemDriverMock, 3000L);
    } catch (int exception) {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void runTests() {
    UNITY_BEGIN();
    RUN_TEST(testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork);
    RUN_TEST(testAwaitNetworkAvailabilityThrowsIfModemFailsToFindANetwork);
    UNITY_END();
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