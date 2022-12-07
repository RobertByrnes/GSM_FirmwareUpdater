#include <Arduino.h>
#include <config.h>
#include <ConfigParser.h>
#include <GSM_FirmwareUpdater.h>


TinyGsm modem(SIM808_SERIAL);
CellularNetwork network(
  ESP32_GSM_APN.c_str(), 
  ESP32_GSM_GPRS_USER.c_str(),
  ESP32_GSM_GPRS_PASSWORD.c_str(),
  modem
);

// HTTP(S) Client
#if defined GSM_SECURE_FIRMWARE_UPDATER
TinyGsmClientSecure client(modem);
#else
TinyGsmClient client(modem);
#endif 

// Firmware Updates
GSM_FirmwareUpdater update;

void setup()
{
  Serial.begin(115200);
  ConfigParser::lsAll();
  ESP32_GSM_FIRMWARE_VERSION = ConfigParser::findString(F("ESP32_GSM_FIRMWARE_VERSION"));
  ESP32_GSM_BAUD_RATE = ConfigParser::findInt(F("ESP32_GSM_BAUD_RATE"));
  ESP32_GSM_LED_PIN = ConfigParser::findInt(F("ESP32_GSM_LED_PIN"));
  ESP32_GSM_MODEM_POWER_PIN = ConfigParser::findInt(F("ESP32_GSM_MODEM_POWER_PIN"));
  ESP32_GSM_MODEM_RESET_PIN = ConfigParser::findInt(F("ESP32_GSM_MODEM_RESET_PIN"));
  ESP32_GSM_MODEM_POWER_ON_PIN = ConfigParser::findInt(F("ESP32_GSM_MODEM_POWER_ON_PIN"));
  ESP32_GSM_MODEM_RX_PIN = ConfigParser::findInt(F("ESP32_GSM_MODEM_RX_PIN"));
  ESP32_GSM_MODEM_TX_PIN = ConfigParser::findInt(F("ESP32_GSM_MODEM_TX_PIN"));
  ESP32_GSM_APN = ConfigParser::findString(F("ESP32_GSM_APN"));
  ESP32_GSM_GPRS_USER = ConfigParser::findString(F("ESP32_GSM_GPRS_USER"));
  ESP32_GSM_GPRS_PASSWORD = ConfigParser::findString(F("ESP32_GSM_GPRS_PASSWORD"));
  ESP32_GSM_SIM_PIN = ConfigParser::findString(F("ESP32_GSM_SIM_PIN"));
  ESP32_GSM_UPDATE_HOST = ConfigParser::findString(F("ESP32_GSM_UPDATE_HOST"));
  ESP32_GSM_PORT = ConfigParser::findInt(F("ESP32_GSM_PORT"));
  ESP32_GSM_UPDATE_URL = ConfigParser::findString(F("ESP32_GSM_UPDATE_URL"));
  ESP32_GSM_UPDATE_VERSION_FILE_URL = ConfigParser::findString(F("ESP32_GSM_UPDATE_VERSION_FILE_URL"));
  ESP32_GSM_WIFI_SSID = ConfigParser::findString(F("ESP32_GSM_WIFI_SSID"));
  ESP32_GSM_WIFI_PASSWORD = ConfigParser::findString(F("ESP32_GSM_WIFI_PASSWORD"));
  ESP32_GSM_STREAM_TIMEOUT = ConfigParser::findInt(F("ESP32_GSM_STREAM_TIMEOUT"));

  log_i("ESP32_GSM_FIRMWARE_VERSION=%s", ESP32_GSM_FIRMWARE_VERSION.c_str());
  log_i("ESP32_GSM_BAUD_RATE=%u", ESP32_GSM_BAUD_RATE);
  log_i("ESP32_GSM_LED_PIN=%u", ESP32_GSM_LED_PIN);
  log_i("ESP32_GSM_APN=%s", ESP32_GSM_APN.c_str());
  log_i("ESP32_GSM_GPRS_USER=%s", ESP32_GSM_GPRS_USER.c_str());
  log_i("ESP32_GSM_GPRS_PASSWORD=%s", ESP32_GSM_GPRS_PASSWORD.c_str());
  log_i("ESP32_GSM_SIM_PIN=%s", ESP32_GSM_SIM_PIN.c_str());
  log_i("ESP32_GSM_UPDATE_HOST=%s", ESP32_GSM_UPDATE_HOST.c_str());
  log_i("ESP32_GSM_PORT=%u", ESP32_GSM_PORT);
  log_i("ESP32_GSM_UPDATE_URL=%s", ESP32_GSM_UPDATE_URL.c_str());
  log_i("ESP32_GSM_UPDATE_VERSION_FILE_URL=%s", ESP32_GSM_UPDATE_VERSION_FILE_URL.c_str());
  log_i("ESP32_GSM_WIFI_SSID=%s", ESP32_GSM_WIFI_SSID.c_str());
  log_i("ESP32_GSM_WIFI_PASSWORD=%s", ESP32_GSM_WIFI_PASSWORD.c_str());
  log_i("ESP32_GSM_STREAM_TIMEOUT=%u", ESP32_GSM_STREAM_TIMEOUT);


  pinMode(ESP32_GSM_LED_PIN, OUTPUT);
  delay(5000);

  // Initialise modem
  log_i("Initializing modem...");

  network.setup_GPIO(
    ESP32_GSM_MODEM_POWER_PIN,
    ESP32_GSM_MODEM_RESET_PIN,
    ESP32_GSM_MODEM_POWER_ON_PIN
  );

  network.initSim(
    ESP32_GSM_SIM_PIN.c_str(),
    ESP32_GSM_BAUD_RATE,
    ESP32_GSM_MODEM_RX_PIN,
    ESP32_GSM_MODEM_TX_PIN,
    SIM808_SERIAL
  );
  
  network._cellnet.restart();

  delay(1000);

  update.setConfig(
    ESP32_GSM_UPDATE_URL,
    ESP32_GSM_UPDATE_HOST,
    ESP32_GSM_PORT
  );

  update.setCRC(0xd108c734);
  update.setTimeout(ESP32_GSM_STREAM_TIMEOUT);
  update.spiffsInit();
  update.updateFirmware(client, network);
}

void loop() { update.updateFirmware(client, network); }
