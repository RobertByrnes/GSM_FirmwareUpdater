#if not defined(MOCK_TINY_GSM_H)
#define MOCK_TINY_GSM_H

#include <Arduino.h>
#include <Emulator.h>

enum MockRegStatus {
  MOCK_REG_NO_RESULT    = -1,
  MOCK_REG_UNREGISTERED = 0,
  MOCK_REG_SEARCHING    = 2,
  MOCK_REG_DENIED       = 3,
  MOCK_REG_OK_HOME      = 1,
  MOCK_REG_OK_ROAMING   = 5,
  MOCK_REG_UNKNOWN      = 4,
};

class MockTinyGsm : public Emulator {
public:
    explicit MockTinyGsm(Stream& stream) {}
    ~MockTinyGsm() {}
    inline bool init() { return this->mock<bool>("init"); }
    inline MockRegStatus getRegistrationStatus() { return this->mock<MockRegStatus>("getRegistrationStatus"); }
    inline bool waitForNetwork(uint32_t timeout_ms, bool check_signal=false) { return this->mock<bool>("waitForNetwork"); }
    bool gprsConnect(const char* apn, const char* user, const char* pwd) { return this->mock<bool>("gprsConnect"); }
    String getSimCCID() { return this->mock<String>("getSimCCID"); }
    String getIMEI() { return this->mock<String>("getIMEI"); }
    String getOperator() { return this->mock<String>("getOperator"); }
    inline IPAddress localIP() { return this->mock<IPAddress>("localIP"); }
    inline int16_t getSignalQuality() { return this->mock<int16_t>("getSignalQuality"); }
    inline String getModemName() { return this->mock<String>("getModemName"); }
    inline String getModemInfo() { return this->mock<String>("getModemInfo"); }
    inline bool isNetworkConnected() { return this->mock<bool>("isNetworkConnected"); }
};

class MockTinyGsmClient {
public:
  MockTinyGsmClient(MockTinyGsm& mockModemDriver) {}
  ~MockTinyGsmClient() {}
};

#endif
