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

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
// #define GSM_AUTOBAUD_MIN 9600
// #define GSM_AUTOBAUD_MAX 115200
#define BAUD_RATE 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// flag to force SSL client authentication, if needed
// #define TINY_GSM_SSL_CLIENT_AUTHENTICATION