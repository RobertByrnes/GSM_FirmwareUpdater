#include <emulation.h>
#include <modem.hpp>
#include <Mocks/MockTinyGsm.hpp>


#define MODEM_UART Serial
MockTinyGsm modemDriverMock(MODEM_UART);
Modem<MockTinyGsm> modemClass;
const char *apn = "pkp18-inet";
const char *gprsUser = "";
const char *gprsPass = "";
uint16_t ledPin = 13;


void setUp(void) {
    modemDriverMock.reset();
}

void tearDown(void) {}

void testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("waitForNetwork", true);
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
        modemDriverMock.returns("waitForNetwork", false);
        modemClass.awaitNetworkAvailability(modemDriverMock, 3000L);
    } catch (int error) {
        exceptionThrown = true;
    }
    TEST_ASSERT_TRUE(exceptionThrown);
}

void testconnectToAPNDoesNotThrowIfModemConnects()
{
    bool exceptionThrown = false;
    try {
        modemDriverMock.returns("gprsConnect", true);
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
        modemDriverMock.returns("gprsConnect", false);
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
        modemDriverMock.returns("isNetworkConnected", true);
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
        modemDriverMock.returns("isNetworkConnected", false);
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

// void testSetupPMUReturnsTrueIfWireReturnsZero()
// {
//     TEST_ASSERT_TRUE(modemClass.setupPMU(Wire));
// }

// void testSetupPMUReturnsFalseIfWireReturnsNoneZero()
// {
//     TEST_ASSERT_FALSE(modemClass.setupPMU(Wire));
// }

void testConnectReturnsTrueIfModemConnectedToAPN()
{
    modemDriverMock.returns("waitForNetwork", true);
    modemDriverMock.returns("gprsConnect", true);
    modemDriverMock.returns("isNetworkConnected", true);
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
    modemDriverMock.returns("waitForNetwork", false);
    bool connected = modemClass.connect(
        modemDriverMock, 
        apn, 
        gprsUser, 
        gprsPass, 
        ledPin
    ); 
    
    TEST_ASSERT_FALSE(connected);
}

void runTests() {
    // FILE *fp = freopen("output.txt", "a", stdout);
    UNITY_BEGIN();
    RUN_TEST(testAwaitNetworkAvailabilityDoesNotThrowIfModemFindsANetwork);
    RUN_TEST(testAwaitNetworkAvailabilityThrowsIfModemFailsToFindANetwork);
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
