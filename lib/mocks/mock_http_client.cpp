#include <Arduino.h>
#include <PsuedoType.hpp>

static const int MOCK_HTTP_SUCCESS = 0;
static const int MOCK_HTTP_ERROR_CONNECTION_FAILED = -1;
static const int MOCK_HTTP_ERROR_API = -2;
static const int MOCK_HTTP_ERROR_TIMED_OUT = -3; 
static const int MOCK_HTTP_ERROR_INVALID_RESPONSE = -4;

class MockHttpClient : public PsuedoTypeBaseClass, public Client {
    public:
    // Mock return vals
    int _returnIntegerType = 0;
    String _StringReturnType = String((const char*)NULL);

    MockHttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort = 443) {}
    MockHttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort = 443) {}
    MockHttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = 443) {}
    ~MockHttpClient() {}

    // client methods
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
    operator bool() { return bool(true); }; // client end

    // Mock methods
    int responseStatusCode() { return _returnIntegerType; }
    String responseBody() { return _StringReturnType; }

    // Mock set up methods
    void setIntegerReturnType(int intVal) { _returnIntegerType = intVal; }
    void setStringReturnType(const char * stringVal) { _StringReturnType = String(stringVal); }
};
