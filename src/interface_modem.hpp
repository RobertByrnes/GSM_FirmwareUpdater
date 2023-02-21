#pragma once

#ifndef MODEM_INTERFACE_H
#define MODEM_INTERFACE_H

#define TINY_GSM_MODEM_SIM800
#include <TinyGSM.h>

class ModemInterface {
    public:
    ModemInterface() {}
    virtual ~ModemInterface() {}
    virtual void setupModem() = 0;
    virtual bool connect(TinyGsm &sim_modem, const char *apn, const char *gprs_user, const char *gprs_pass, uint16_t ledPin=0) {}
    virtual void awaitNetworkAvailability(TinyGsm &sim_modem, long wait=15000L) {}
    virtual void connectModemToGPRS(TinyGsm &sim_modem) {}
    virtual void connectToAPN(TinyGsm &sim_modem, const char *apn, const char *gprs_user, const char *gprs_pass, uint16_t ledPin=0) {}
    virtual void logConnectionInformation(TinyGsm &sim_modem) {}
    virtual void logModemInformation(TinyGsm &sim_modem) {}
    virtual bool setupPMU() {}
};

#endif
