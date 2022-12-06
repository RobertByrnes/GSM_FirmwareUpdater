#ifndef CELLULAR_NETWORK_H
#define CELLULAR_NETWORK_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <modem.h>
#include <TinyGsm.h>

class CellularNetwork
{
public:
    const char *_apn;
    const char *_gprs_user;
    const char *_gprs_password;
    TinyGsm _cellnet;

    CellularNetwork(
        const char *apn,
        const char *gprsUser,
        const char *gprsPassword,
        TinyGsm &modem
    );

    ~CellularNetwork();

    bool initSim(
        const char *simPin,
        int &baudRate,
        uint8_t &rx,
        uint8_t &tx,
        HardwareSerial &serial
    );

    bool connectNetwork();

    void setup_GPIO(
        uint8_t &pwKey,
        uint8_t &rst,
        uint8_t &pwOn
    );
};

#endif