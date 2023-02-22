#pragma once

#ifndef MOCKTINYGSM_H
#define MOCKTINYGSM_H

#include <ArduinoFake.h>
#include <PsuedoType.hpp>

enum MockRegStatus {
  MOCK_REG_NO_RESULT    = -1,
  MOCK_REG_UNREGISTERED = 0,
  MOCK_REG_SEARCHING    = 2,
  MOCK_REG_DENIED       = 3,
  MOCK_REG_OK_HOME      = 1,
  MOCK_REG_OK_ROAMING   = 5,
  MOCK_REG_UNKNOWN      = 4,
};

class MockTinyGsm : public PsuedoTypeBaseClass {
public:
    explicit MockTinyGsm(Stream& stream) {}
    ~MockTinyGsm() {}
    inline bool init() { return this->findReturnType<bool>("init"); }
    inline MockRegStatus getRegistrationStatus() { this->findReturnType<bool>("getRegistrationStatus"); }
    inline bool waitForNetwork(uint32_t timeout_ms, bool check_signal=false) { return this->findReturnType<bool>("waitForNetwork"); }
    bool gprsConnect(const char* apn, const char* user, const char* pwd) { return this->findReturnType<bool>("gprsConnect"); }
    String getSimCCID() { return this->findReturnType<String>("getSimCCID"); }
    String getIMEI() { return this->findReturnType<String>("getIMEI"); }
    String getOperator() { return this->findReturnType<String>("getOperator"); }
    inline IPAddress localIP() { return this->findReturnType<IPAddress>("localIP"); }
    inline int16_t getSignalQuality() { return this->findReturnType<int16_t>("getSignalQuality"); }
    inline String getModemName() { this->findReturnType<String>("getModemName"); }
    inline String getModemInfo() { this->findReturnType<String>("getModemInfo"); }
    inline bool isNetworkConnected() { this->findReturnType<bool>("isNetworkConnected"); }
};

class MockTinyGsmClient {
public:
  MockTinyGsmClient(MockTinyGsm& mockModemDriver) {}
  ~MockTinyGsmClient() {}
};

#endif
