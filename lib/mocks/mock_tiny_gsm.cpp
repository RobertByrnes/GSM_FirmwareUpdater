#include <Arduino.h>
#include <IPAddress.h>

enum RegStatus {
  REG_NO_RESULT    = -1,
  REG_UNREGISTERED = 0,
  REG_SEARCHING    = 2,
  REG_DENIED       = 3,
  REG_OK_HOME      = 1,
  REG_OK_ROAMING   = 5,
  REG_UNKNOWN      = 4,
};

class MockTinyGsm {
public:
    explicit MockTinyGsm(Stream& stream) {}
    ~MockTinyGsm() {}
    bool _booleanReturnType = true;
    String _stringReturnType = String();
    RegStatus _fakeRegStatus = RegStatus::REG_UNREGISTERED;
    int16_t _fakeSignalQuality = 0;
    IPAddress _ipAddress = IPAddress(0,0,0,0);
    void setBooleanReturnType(bool boolVal) { _booleanReturnType = boolVal; }
    void setStringReturnType(const char * string) { _stringReturnType = string; }
    void setFakeRegStatus(RegStatus status) { _fakeRegStatus = status; }
    void setFakeSignalQuality(int16_t quality) { _fakeSignalQuality = quality; }
    void setFakeIpAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
        _ipAddress = IPAddress(first_octet, second_octet, third_octet, fourth_octet);
    }
    inline bool init() { return _booleanReturnType; }
    inline RegStatus getRegistrationStatus() { return _fakeRegStatus; }
    inline bool waitForNetwork(uint32_t timeout_ms, bool check_signal=false) { return _booleanReturnType; }
    bool gprsConnect(const char* apn, const char* user, const char* pwd) { return _booleanReturnType; }
    String getSimCCID() { return _stringReturnType; }
    String getIMEI() { return _stringReturnType; }
    String getOperator() { return _stringReturnType; }
    inline IPAddress localIP() { return _ipAddress; }
    inline int16_t getSignalQuality() { return _fakeSignalQuality; }
    inline String getModemName() { return _stringReturnType; }
    inline String getModemInfo() { return _stringReturnType; }
    inline bool isNetworkConnected() { return _booleanReturnType; }
};

class MockTinyGsmClient {
public:
  MockTinyGsmClient(MockTinyGsm& mockModemDriver) {}
  ~MockTinyGsmClient() {}
};
