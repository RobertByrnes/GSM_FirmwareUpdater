#include <unity.h>
#include <modem.hpp>

#define MODEM_UART Serial1

TinyGsm sim_modem(MODEM_UART);
const char *apn = "pkp18-inet";
const char *gprs_user = "";
const char *gprs_pass = "";
uint16_t ledPin = 13;

void setUp(void) { 
    Modem::setupModem();
    MODEM_UART.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
}

void tearDown(void) {
    sim_modem.init();
}

void test_setupModem(void)
{
    // Check if the pin values have been set as expected
    TEST_ASSERT_EQUAL(HIGH, digitalRead(MODEM_RST));
    TEST_ASSERT_EQUAL(HIGH, digitalRead(MODEM_POWER_ON));
    TEST_ASSERT_EQUAL(HIGH, digitalRead(MODEM_PWRKEY));
    TEST_ASSERT_EQUAL(LOW, digitalRead(LED_PIN));
}

void test_modemStartsNotConnected(void)
{
    TEST_ASSERT_EQUAL(sim_modem.getRegistrationStatus(), RegStatus::REG_NO_RESULT);
}

// void test_awaitNetworkAvailability(void)
// {
//     bool exceptionThrown = false;
//     try {
//         Modem::awaitNetworkAvailability(sim_modem, 10000L);
//     } catch (uint8_t error) {
//         exceptionThrown = true;
//     }
//     TEST_ASSERT_FALSE(exceptionThrown);
// }

void test_connectModemToGPRS(void)
{
    // TODO: Write a test for connectModemToGPRS
}

void test_connectToAPN(void)
{
    // TODO: Write a test for connectToAPN
}

void test_logConnectionInformationWithoutError(void)
{
    bool errored = false;
    try {
        Modem::logConnectionInformation(sim_modem);
    } catch (...) {
        errored = true;
    }
    TEST_ASSERT_EQUAL(errored, false);
}

void test_logModemInformationWithoutError(void)
{
    bool errored = false;
    try {
        Modem::logModemInformation(sim_modem);
    } catch (...) {
        errored = true;
    }
    TEST_ASSERT_EQUAL(errored, false);
}

void test_setupPMU(void)
{
    TEST_ASSERT_TRUE(Modem::setupPMU());
}

void test_connect(void)
{
    Modem::connect(
        sim_modem, 
        apn, 
        gprs_user, 
        gprs_pass, 
        ledPin
    );
    TEST_ASSERT_EQUAL_STRING(sim_modem.getOperator().c_str(), "");
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_setupModem);
    RUN_TEST(test_setupPMU);
    RUN_TEST(test_modemStartsNotConnected);
    // RUN_TEST(test_awaitNetworkAvailability);
    // RUN_TEST(test_connectModemToGPRS);
    // RUN_TEST(test_connectToAPN);
    RUN_TEST(test_connect);
    RUN_TEST(test_logModemInformationWithoutError);
    RUN_TEST(test_logConnectionInformationWithoutError);
}

void loop() {}
