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
    inline bool init() { this->findReturnType("init"); return _booleanReturnType; }
    // inline MockRegStatus getRegistrationStatus() { this->findReturnType("getRegistrationStatus"); return }
    inline bool waitForNetwork(uint32_t timeout_ms, bool check_signal=false) { this->findReturnType("waitForNetwork"); return _booleanReturnType; }
    bool gprsConnect(const char* apn, const char* user, const char* pwd) { this->findReturnType("gprsConnect"); return _booleanReturnType; }
    String getSimCCID() { this->findReturnType("getSimCCID"); return _StringReturnType; }
    String getIMEI() { this->findReturnType("getIMEI"); return _StringReturnType; }
    String getOperator() { this->findReturnType("getOperator"); return _StringReturnType; }
    inline IPAddress localIP() { this->findReturnType("localIP"); return _ipAddressReturnType; }
    inline int16_t getSignalQuality() { this->findReturnType("getSignalQuality"); return _intReturnType; }
    inline String getModemName() { this->findReturnType("getModemName"); return _StringReturnType; }
    inline String getModemInfo() { this->findReturnType("getModemInfo"); return _StringReturnType; }
    inline bool isNetworkConnected() { this->findReturnType("isNetworkConnected"); return _booleanReturnType; }
};

class MockTinyGsmClient {
public:
  MockTinyGsmClient(MockTinyGsm& mockModemDriver) {}
  ~MockTinyGsmClient() {}
};

#endif
