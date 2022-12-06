#include "CellularNetwork.h"

// Constructor
CellularNetwork::CellularNetwork(
  const char *apn, 
  const char *gprsUser, 
  const char *gprsPassword, 
  TinyGsm &modem
): _apn(apn), _gprs_user(gprsUser), _gprs_password(gprsPassword), _cellnet(modem) {}

// Destructor
CellularNetwork::~CellularNetwork() {}

/**
 * @brief Make up to 5 attempts to connect to the mobile network.
 * 
 * @param simPin const char *
 * @param baudRate int &
 * @param rx uint8_t &
 * @param tx uint8_t &
 * @param serial HardwareSerial &
 * 
 * @return bool
 */
bool CellularNetwork::initSim(
  const char *simPin,
  int &baudRate,
  uint8_t &rx,
  uint8_t &tx,
  HardwareSerial &serial
) { 
  serial.begin(baudRate, SERIAL_8N1, rx, tx); // Set GSM module baud rate and UART pins
  delay(3000);

  _cellnet.restart();

  if (strlen(simPin) && _cellnet.getSimStatus() != 3) { // Unlock SIM card with a PIN if needed
    _cellnet.simUnlock(simPin);
  }

  return true;
}

/**
 * @brief Make up to 5 attempts to connect to the mobile network.
 * 
 * @return bool
 */
bool CellularNetwork::connectNetwork()
{
  int i = 0;
  while (i < 5) {
    ++i;
    if (!_cellnet.gprsConnect(_apn, _gprs_user, _gprs_password)) {
      log_i("Attempting to connect to %s", _apn);
      delay(3000);
    } else {
      // log_i("Connected to %s", _apn);
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
void CellularNetwork::setup_GPIO(
  uint8_t &pwKey,
  uint8_t &rst,
  uint8_t &pwOn)
{
  pinMode(pwKey, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(pwOn, OUTPUT);
  digitalWrite(pwKey, LOW);
  digitalWrite(rst, HIGH);
  digitalWrite(pwOn, HIGH);
}