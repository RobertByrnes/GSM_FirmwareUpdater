#include <unity.h>

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <modem.hpp>
#include <mock_tiny_gsm.hpp>

#define MODEM_UART Serial1
MockTinyGsm modemDriverMock(MODEM_UART);
Modem<MockTinyGsm> modemClass;
const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
uint16_t ledPin = 13;


void setUp(void) {}
void tearDown(void) {}

void testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(true);
        modemClass.awaitNetworkAvailability(modemDriverMock, 3000L);
    } catch (uint8_t error) {
        exceptionThrown = true;
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void testAwaitNetworkAvailabilityThrowsIfModemFailsToFindANetwork()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(false);
        modemClass.awaitNetworkAvailability(modemDriverMock, 3000L);
    } catch (uint8_t error) {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testconnectToAPNDoesNotThrowIfModemConnects()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(true);
        modemClass.connectToAPN(modemDriverMock, apn, gprsUser, gprsPass);
    } catch (...) {
        exceptionThrown = true;
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void testconnectToAPNThrowsIfModemFailsToConnect()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(false);
        modemClass.connectToAPN(modemDriverMock, apn, gprsUser, gprsPass);
    } catch (...) {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testVerifyConnectedDoesNotThrowIfModemIsConnected()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(true);
        modemClass.verifyConnected(modemDriverMock);
    } catch (...) {
        exceptionThrown = true;
    }
    TEST_ASSERT_FALSE(exceptionThrown);
}

void testVerifyConnectThrowsIfModemNotConnected()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns(false);
        modemClass.verifyConnected(modemDriverMock);
    } catch (...) {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testLogConnectionInformationWithoutError()
{
    bool errored = false;
    try {
        modemClass.logConnectionInformation(modemDriverMock);
    } catch (...) {
        errored = true;
    }
    TEST_ASSERT_EQUAL(errored, false);
}

void testLogModemInformationWithoutError()
{
    bool errored = false;
    try {
        modemClass.logModemInformation(modemDriverMock);
    } catch (...) {
        errored = true;
    }
    TEST_ASSERT_EQUAL(errored, false);
}

void testSetupPMUReturnsTrueIfWireReturnsZero()
{
    modemDriverMock.returns(true);
    TEST_ASSERT_TRUE(modemClass.setupPMU());
}

void testSetupPMUReturnsFalseIfWireReturnsNoneZero()
{
    modemDriverMock.returns(false);
    TEST_ASSERT_FALSE(modemClass.setupPMU());
}

void testConnectReturnsTrueIfModemConnectedToAPN()
{
    modemDriverMock.returns(true);
    bool connected = modemClass.connect(
        modemDriverMock, 
        apn, 
        gprsUser, 
        gprsPass, 
        ledPin
    );
    
    TEST_ASSERT_TRUE(connected);
}

void testConnectReturnsFalseIfModemFailsToConnect()
{
    modemDriverMock.returns(false);
    bool connected = modemClass.connect(
        modemDriverMock, 
        apn, 
        gprsUser, 
        gprsPass, 
        ledPin
    ); 
    
    TEST_ASSERT_FALSE(connected);
}

void setup()
{ 
    UNITY_BEGIN();
    // RUN_TEST(testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork);
    // RUN_TEST(testAwaitNetworkAvailabilityThrowsIfModemFailsToFindANetwork);
    RUN_TEST(testconnectToAPNDoesNotThrowIfModemConnects);
    RUN_TEST(testconnectToAPNThrowsIfModemFailsToConnect);
    RUN_TEST(testVerifyConnectedDoesNotThrowIfModemIsConnected);
    RUN_TEST(testVerifyConnectThrowsIfModemNotConnected);
    RUN_TEST(testConnectReturnsTrueIfModemConnectedToAPN);
    RUN_TEST(testConnectReturnsFalseIfModemFailsToConnect);
    RUN_TEST(testLogConnectionInformationWithoutError);
    RUN_TEST(testLogModemInformationWithoutError);
    // RUN_TEST(testSetupPMUReturnsTrueIfWireReturnsZero);
    // RUN_TEST(testSetupPMUReturnsFalseIfWireReturnsNoneZero);
    UNITY_END();
}

void loop() {}
