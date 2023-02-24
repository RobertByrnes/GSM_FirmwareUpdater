#include <mock_tiny_gsm.hpp>


template<class T>
class MockSSLClient : public Client  {
public:
    MockSSLClient() {}
    MockSSLClient(T* client) {}
    ~MockSSLClient() {}
    int connect(IPAddress ip, uint16_t port) { return 0; };
    int connect(const char *host, uint16_t port) { return 0; };
    size_t write(uint8_t) { return 0; };
    size_t write(const uint8_t *buf, size_t size) { return 0; };
    int available() { return 0; };
    int read() { return 0; };
    int read(uint8_t *buf, size_t size) { return 0; };
    int peek() { return 0; };
    void flush() {};
    void stop() {};
    uint8_t connected() { return 0; };
    operator bool() { return bool(true); };
};
