// #include <Arduino.h>
// #include <GSM_FirmwareUpdater.h>

// #define TINY_GSM_MODEM_SIM800 // Modem is SIM800

// #include <TinyGSM.h>

// #define TINY_GSM_RX_BUFFER              1024 // Set RX buffer to 1Kb
// #define SIM800_SERIAL                   Serial1
// #define I2C_SDA                         21 // Serial data
// #define I2C_SCL                         22 // Serial clock
// #define IP5306_ADDR                     0x75 // Initiate communication
// #define IP5306_REG_SYS_CTL0             0x00 // Begin write
// // #define GSM_SECURE_FIRMWARE_UPDATER
// #define ESP32_GSM_LED_PIN               13
// #define ESP32_GSM_SIM_PIN               ""

// int ESP32_GSM_BAUD_RATE = 115200;
// uint8_t ESP32_GSM_MODEM_POWER_PIN = 4;
// uint8_t ESP32_GSM_MODEM_RESET_PIN = 5;
// uint8_t ESP32_GSM_MODEM_POWER_ON_PIN = 23;
// uint8_t ESP32_GSM_MODEM_RX_PIN = 26;
// uint8_t ESP32_GSM_MODEM_TX_PIN = 27;
// uint32_t ESP32_GSM_STREAM_TIMEOUT = 260000;
// uint16_t ESP32_GSM_PORT = 80;

// std::string ESP32_GSM_UPDATE_HOST = "somesite.ontheweb.com";
// std::string ESP32_GSM_UPDATE_BINARY_FILE_URL = "/bin/firmware.bin";
// std::string ESP32_GSM_UPDATE_VERSION_FILE_URL = "/bin/firmware.txt";

// std::string ESP32_GSM_FIRMWARE_VERSION = "0.0.1";

// TinyGsm modem(SIM800_SERIAL);
// CellularNetwork network(
//   "***APN***", 
//   "***USER***",
//   "***PASSWORD***",
//   modem
// );

// // HTTP(S) Client
// #if defined GSM_SECURE_FIRMWARE_UPDATER
// TinyGsmClientSecure client(modem);
// #else
// TinyGsmClient client(modem);
// #endif 

// // Firmware Updates
// GSM_FirmwareUpdater update;

// void setup()
// {
//   Serial.begin(115200);

//   pinMode(ESP32_GSM_LED_PIN, OUTPUT);
//   delay(5000);

//   // Initialise modem
//   log_i("Initializing modem...");

//   network.setup_GPIO(
//     ESP32_GSM_MODEM_POWER_PIN,
//     ESP32_GSM_MODEM_RESET_PIN,
//     ESP32_GSM_MODEM_POWER_ON_PIN
//   );

//   network.initSim(
//     ESP32_GSM_SIM_PIN,
//     ESP32_GSM_BAUD_RATE,
//     ESP32_GSM_MODEM_RX_PIN,
//     ESP32_GSM_MODEM_TX_PIN,
//     SIM800_SERIAL
//   );
  
//   network._cellnet.restart();

//   delay(1000);

//   update.configure(ESP32_GSM_FIRMWARE_VERSION);

//   update.setCRC(0xd108c734);
//   update.setTimeout(ESP32_GSM_STREAM_TIMEOUT);
//   update.updateFirmware(client, network);
// }

// void loop() { update.updateFirmware(client, network); }
