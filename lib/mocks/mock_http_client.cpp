#include <Arduino.h>
#include <Emulator.h>

static const int MOCK_HTTP_SUCCESS = 0;
static const int MOCK_HTTP_ERROR_CONNECTION_FAILED = -1;
static const int MOCK_HTTP_ERROR_API = -2;
static const int MOCK_HTTP_ERROR_TIMED_OUT = -3; 
static const int MOCK_HTTP_ERROR_INVALID_RESPONSE = -4;

class MockHttpClient : public Emulator, public Client {
    public:
    MockHttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort = 443) {}
    MockHttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort = 443) {}
    MockHttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = 443) {}
    ~MockHttpClient() {}

    // client methods
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
    operator bool() { return bool(true); }; // client end

    // Mock methods
    int responseStatusCode() { return this->mock<int>("responseStatusCode"); };
    String responseBody() { return this->mock<String>("responseBody"); };
};
