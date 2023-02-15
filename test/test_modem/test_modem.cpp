#include <gtest/gtest.h>

// TEST(...)
// TEST_F(...)

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <modem.hpp>

#define MODEM_UART Serial1
Modem modem;
TinyGsm sim_modem(MODEM_UART);
const char *apn = "pkp18-inet";
const char *gprs_user = "";
const char *gprs_pass = "";
uint16_t ledPin = 13;

void setUp(void) {
    modem.setupModem();
    MODEM_UART.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
}

void tearDown(void) {
    sim_modem.init();
}

TEST(IsModemTest, setupModem)
{
    setUp();
    // Check if the pin values have been set as expected
    EXPECT_EQ(HIGH, digitalRead(MODEM_RST));
    EXPECT_EQ(HIGH, digitalRead(MODEM_POWER_ON));
    EXPECT_EQ(HIGH, digitalRead(MODEM_PWRKEY));
    EXPECT_EQ(LOW, digitalRead(MODEM_LED_PIN));
    tearDown();
}

TEST(IsModemTest, modemStartsNotConnected)
{
    setUp();
    EXPECT_EQ(sim_modem.getRegistrationStatus(), RegStatus::REG_NO_RESULT);
    tearDown();
}

TEST(IsModemTest, awaitNetworkAvailability)
{
    bool exceptionThrown = false;
    try {
        modem.awaitNetworkAvailability(sim_modem, 30000L);
    } catch (uint8_t error) {
        exceptionThrown = true;
    }
    ASSERT_FALSE(exceptionThrown);
}

TEST(IsModemTest, connectModemToGPRS)
{
    setUp();
    // TODO: Write a test for connectModemToGPRS
    tearDown();
}

TEST(IsModemTest, connectToAPN)
{
    setUp();
    // TODO: Write a test for connectToAPN
    tearDown();
}

TEST(IsModemTest, logConnectionInformationWithoutError)
{
    setUp();
    bool errored = false;
    try {
        modem.logConnectionInformation(sim_modem);
    } catch (...) {
        errored = true;
    }
    EXPECT_EQ(errored, false);
    tearDown();
}

TEST(IsModemTest, logModemInformationWithoutError)
{
    setUp();
    bool errored = false;
    try {
        modem.logModemInformation(sim_modem);
    } catch (...) {
        errored = true;
    }
    EXPECT_EQ(errored, false);
    tearDown();
}

TEST(IsModemTest, setupPMU)
{
    setUp();
    EXPECT_TRUE(modem.setupPMU());
    tearDown();
}

TEST(IsModemTest, connect)
{
    setUp();
    modem.connect(
        sim_modem, 
        apn, 
        gprs_user, 
        gprs_pass, 
        ledPin
    );
    EXPECT_STREQ(sim_modem.getOperator().c_str(), "");
    tearDown();
}

void setup()
{   
    Serial.begin(115200);
    ::testing::InitGoogleTest();
}

void loop()
{
  // Run tests
  if (RUN_ALL_TESTS())
  ;

  // sleep for 1 sec
  delay(1000);
}
