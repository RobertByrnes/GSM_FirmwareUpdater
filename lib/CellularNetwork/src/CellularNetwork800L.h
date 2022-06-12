#ifndef CELLULAR_NETWORK_H
#define CELLULAR_NETWORK_H

#include <Arduino.h>
#include <modem.h>
#include <credentials.h>
#include <HardwareSerial.h>
#include <TinyGsmClient.h>


class CellularNetwork800L
{

public:
    const char *apn;
    const char *gprs_user;
    const char *gprs_password;
    TinyGsm connection;

    CellularNetwork800L(const char *apn, const char *gprs_user, const char *gprs_password, TinyGsm &modem);

    ~CellularNetwork800L();

    bool initSim(const char *simPin);
    bool connectNetwork();

private:
    void setup_GPIO();
};

#endif