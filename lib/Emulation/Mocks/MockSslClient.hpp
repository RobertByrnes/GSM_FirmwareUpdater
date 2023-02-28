#if not defined(MOCK_SSL_CLIENT_H)
#define MOCK_SSL_CLIENT_H

#include <Arduino.h>
#include <Emulator.h>

template<class T>
class MockSSLClient : public Emulator, public Client  {
public:
    MockSSLClient() {}
    MockSSLClient(T* client) {}
    ~MockSSLClient() {}
    int connect(IPAddress ip, uint16_t port) { return this->mock<int>("connect"); };
    int connect(const char *host, uint16_t port) { return this->mock<int>("connect"); };
    size_t write(uint8_t) { return this->mock<size_t>("write"); };
    size_t write(const uint8_t *buf, size_t size) { return this->mock<size_t>("write"); };
    int available() { return this->mock<int>("available"); };
    int read() { return this->mock<int>("read"); };
    int read(uint8_t *buf, size_t size) { return this->mock<int>("read"); };
    int peek() { return this->mock<int>("peek"); };
    void flush() {};
    void stop() {};
    uint8_t connected() { return this->mock<uint8_t>("connected"); };
    operator bool() { return bool(true); };
};

#endif
