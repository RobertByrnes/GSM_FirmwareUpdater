// ESP32 LilyGo-T-Call-SIM800 SIM800L_IP5306_VERSION_20190610 (v1.3) pins definition
#define TINY_GSM_MODEM_SIM800
#define MODEM_RST 5
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 23
#define MODEM_UART_BAUD 9600
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22
#define IP5306_ADDR 0x75
#define IP5306_REG_SYS_CTL0 0x00
#define TINY_GSM_RX_BUFFER 1024
#define MODEM_LED_PIN 13

// SIM808
// #define TINY_GSM_MODEM_SIM808
// #define SIM808_BAUD_RATE 115200
// #define SIM808_TX 17
// #define SIM808_RX 16
// #define SIM808_SERIAL Serial1
// #define MODEM_PWRKEY 15
// #define TINY_GSM_RX_BUFFER 1024
// #define MODEM_LED_PIN 2

#define MODEM_NO_NETWORK_CONN (1)
#define MODEM_NO_GPRS_CONN (2)
#define MODEM_NO_APN_CONN (3)

// Include after ModemDriver definitions
#include <Wire.h>
#include <Logger.h>
#include <TinyGsm.h>
#include <TinyGsmClient.h>

template <class ModemDriver>
class Modem {
    public:
    Modem() {}
    ~Modem() {}
    void setupModem();
    bool connect(ModemDriver &sim_modem, const char *apn, const char *gprs_user, const char *gprs_pass, uint16_t ledPin=0);
    void awaitNetworkAvailability(ModemDriver &sim_modem, long wait=15000L);
    void verifyConnected(ModemDriver &sim_modem);
    void connectToAPN(ModemDriver &sim_modem, const char *apn, const char *gprs_user, const char *gprs_pass, uint16_t ledPin=0);
    void logConnectionInformation(ModemDriver &sim_modem);
    void logModemInformation(ModemDriver &sim_modem);
    #if defined(I2C_SDA) && defined(I2C_SCL) && defined(IP5306_ADDR) && defined(IP5306_REG_SYS_CTL0)
    
    template <typename T>
    bool setupPMU(T wire);
    #endif
};

/**
 * @brief Modem initial setup (cold start)
 * @return void
 */
template<class ModemDriver>
void Modem<ModemDriver>::setupModem()
{
    #if defined(MODEM_RST) // Reset pin high
        pinMode(MODEM_RST, OUTPUT);
        digitalWrite(MODEM_RST, HIGH);
    #endif
    #if defined(MODEM_PWRKEY) // Pull down PWRKEY for more than 1 second according to manual requirements
        pinMode(MODEM_PWRKEY, OUTPUT);
        digitalWrite(MODEM_PWRKEY, HIGH);
        delay(200);
        digitalWrite(MODEM_PWRKEY, LOW);
        delay(1200);
        digitalWrite(MODEM_PWRKEY, HIGH);
    #endif 
    #if defined(MODEM_POWER_ON) // Turn on the Modem power first
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);
    #endif
    #if defined(MODEM_LED_PIN) // Initialize the indicator as an output
    pinMode(MODEM_LED_PIN, OUTPUT);
    digitalWrite(MODEM_LED_PIN, LOW);
    #endif
}

/**
 * @brief 
 * 
 * @param sim_modem 
 * @param apn 
 * @param gprs_user 
 * @param gprs_pass 
 * @param ledPin 
 */
template<class ModemDriver>
bool Modem<ModemDriver>::connect(
    ModemDriver &sim_modem, 
    const char *apn, 
    const char *gprs_user, 
    const char *gprs_pass, 
    uint16_t ledPin
) {
    try {
        this->awaitNetworkAvailability(sim_modem);
        this->connectToAPN(sim_modem, apn, gprs_user, gprs_pass, ledPin);
        this->verifyConnected(sim_modem);
        return true;
    } catch (int error) {
        switch (error) {
            case 1: log_e("The modem could not find a network"); break;
            case 2: log_e("The modem could not connect to the APN"); break;
            case 3: log_e("The modem could not connect to the GPRS network");
        }
    } catch(...) {
        log_e("Errored in Modem::connect and not caught with custom error handler");
    }
    return false;
}

/**
 * @brief Wait for network availability
 *
 * @param sim_modem
 * @throws int MODEM_NO_NETWORK_CONN = 0;
 */
template<class ModemDriver>
void Modem<ModemDriver>::awaitNetworkAvailability(ModemDriver &sim_modem, long wait)
{
    log_i("Waiting for GPRS network");
    if (!sim_modem.waitForNetwork(wait, true)) {
        throw 1;
    }
    log_i("Network available");
}

/**
 * @brief Connect to the GPRS network
 *
 * @param sim_modem
 * @throws int MODEM_NO_GPRS_CONN = 1;
 */
template <class ModemDriver>
void Modem<ModemDriver>::verifyConnected(ModemDriver &sim_modem)
{
    log_i("Connecting to GPRS network, status: %i");
    if (!sim_modem.isNetworkConnected() > 0) {
        throw 2;
    }
    log_i("GPRS connected OK");
}

/**
 * @brief Connect to APN
 * 
 * @param sim_modem 
 * @param apn 
 * @param gprs_user 
 * @param gprs_pass 
 * 
 * @throws int MODEM_NO_APN_CONN = 3;
 */
template<class ModemDriver>
void Modem<ModemDriver>::connectToAPN(
    ModemDriver &sim_modem,
    const char *apn,
    const char *gprs_user,
    const char *gprs_pass,
    uint16_t ledPin
) {
    log_i("Connecting to APN: %s", apn);
    if (!sim_modem.gprsConnect(apn, gprs_user, gprs_pass)) {
        throw 3;
    } else {
        log_i("Connected, ready to send/receive");
    }
    if (ledPin > 0) {
        #ifdef ARDUINO
        digitalWrite(ledPin, HIGH);
        #endif
    }
}

/**
 * @brief When connected to GPRS network log ICCID,
 * IMEI, Operator, Local IP and Signal Quality.
 * 
 * @param sim_modem 
 */
template<class ModemDriver>
void Modem<ModemDriver>::logConnectionInformation(ModemDriver &sim_modem) {  
    log_i("ICCID: %s", sim_modem.getSimCCID().c_str());
    log_i("IMEI: %s", sim_modem.getIMEI().c_str());
    log_i("Operator: %s", sim_modem.getOperator());
    log_i("Local IP: %s", sim_modem.localIP().toString());
    log_i("Signal quality: %i", sim_modem.getSignalQuality());
}

/**
 * @brief Communicate with the modem and log the modem name
 * and firmware version.
 * 
 * @param sim_modem 
 */
template <class ModemDriver>
void Modem<ModemDriver>::logModemInformation(ModemDriver &sim_modem) {
    log_i("Modem Name: %s", sim_modem.getModemName());
    log_i("Modem Info: %s", sim_modem.getModemInfo().c_str());
}


#if defined(I2C_SDA) && defined(I2C_SCL) && defined(IP5306_ADDR) && defined(IP5306_REG_SYS_CTL0)
/**
 * @brief Power configuration for SIM800L_IP5306_VERSION_20190610 (v1.3) board.
 * 
 * @return true 
 * @return false 
 */
template <class ModemDriver>
template <typename T>
bool Modem<ModemDriver>::setupPMU(T wire)
{
    bool en = true;
    wire.begin();
    wire.beginTransmission(IP5306_ADDR);
    wire.write(IP5306_REG_SYS_CTL0);
    if (en) {
        wire.write(0x37);
    } else {
        wire.write(0x35);
    }
    return wire.endTransmission() == 0;
}
#endif
