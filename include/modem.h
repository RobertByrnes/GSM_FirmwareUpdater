
#define BAUD_RATE 115200

// SIM800 pins
#define MODEM_RST 5 // Reset pin
#define MODEM_PWKEY 4 // Enable pin
#define MODEM_POWER_ON 23 // Power pin
#define MODEM_TX 27 // Transmit pin
#define MODEM_RX 26 // Receive pin
#define I2C_SDA 21 // Serial data
#define I2C_SCL 22 // Serial clock

// Aliases Serial1 interface for AT commands to SIM800 module
#define SerialAT Serial1
// #define DUMP_AT_COMMANDS

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Registry config values for I2C
// with the SIM800 modem
#define IP5306_ADDR 0x75 // Initiate communication
#define IP5306_REG_SYS_CTL0 0x00 // Begin write

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS