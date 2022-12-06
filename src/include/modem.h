#define TINY_GSM_MODEM_SIM808 // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SIM808_SERIAL Serial1
#define I2C_SDA 21 // Serial data
#define I2C_SCL 22 // Serial clock
#define IP5306_ADDR 0x75 // Initiate communication
#define IP5306_REG_SYS_CTL0 0x00 // Begin write
#define GSM_INSECURE_FIRMWARE_UPDATER