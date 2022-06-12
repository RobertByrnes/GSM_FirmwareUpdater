#include <Arduino.h>
#include "config.h"
#include <GSMFirmwareUpdater.h>

// User queues
#define LED_PIN 13

// Memory profiling - uncomment to enable
#define PROFILE_MEMORY

// Serial print verbosity - uncomment to enable output (it's a good idea to turn this off in production, unless Serial connection is available)
#define SERIAL_VERBOSE true


// Modem
TinyGsm modem(SerialAT);

// HTTP(S) Client
TinyGsmClientSecure client(modem);

void setup()
{
  Serial.begin(BAUD_RATE);

  pinMode(LED_PIN, OUTPUT);
}

void loop() 
{

}