#include "CellularNetwork800L.h"
#include <TinyGsmClient.h>

// Constructor
CellularNetwork800L::CellularNetwork800L(const char *apn, const char *gprs_user, const char *gprs_password, TinyGsm &modem):
  apn(apn), gprs_user(gprs_user), gprs_password(gprs_password), connection(modem) {}

// Destructor
CellularNetwork800L::~CellularNetwork800L() {}

/**
 * @brief Make up to 5 attempts to connect to the mobile network.
 * 
 * @param simPin const char *
 * 
 * @return bool
 */
bool CellularNetwork800L::initSim(const char *simPin)
{
  this->setup_GPIO();
   
  SerialAT.begin(BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX); // Set GSM module baud rate and UART pins
  delay(3000);

  this->connection.restart();

  if (strlen(simPin) && this->connection.getSimStatus() != 3) { // Unlock SIM card with a PIN if needed
    this->connection.simUnlock(simPin);
  }

  return true;
}

/**
 * @brief Make up to 5 attempts to connect to the mobile network.
 * 
 * @return bool
 */
bool CellularNetwork800L::connectNetwork()
{
  int i = 0;
  while (i <= 5) {
    ++i;
    if (!this->connection.gprsConnect(this->apn, this->gprs_user, this->gprs_password)) {
      delay(3000);
    } else {
      return true;
    }
  }
  return false;
}

/**
 * @brief Set modem enable, reset and power pins.
 * 
 * @return void
 */ 
void CellularNetwork800L::setup_GPIO()
{
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
}