#if not defined(MOCK_HTTP_CLIENT)
#define MOCK_HTTP_CLIENT

#include <Arduino.h>
#include <Emulator.h>

#define HTTP_METHOD_GET    "GET"
#define HTTP_METHOD_POST   "POST"
#define HTTP_METHOD_PUT    "PUT"
#define HTTP_METHOD_PATCH  "PATCH"
#define HTTP_METHOD_DELETE "DELETE"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONTENT_TYPE   "Content-Type"
#define HTTP_HEADER_CONNECTION     "Connection"
#define HTTP_HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HTTP_HEADER_USER_AGENT     "User-Agent"
#define HTTP_HEADER_VALUE_CHUNKED  "chunked"

static const int MOCK_HTTP_SUCCESS = 0;
static const int MOCK_HTTP_ERROR_CONNECTION_FAILED = -1;
static const int MOCK_HTTP_ERROR_API = -2;
static const int MOCK_HTTP_ERROR_TIMED_OUT = -3; 
static const int MOCK_HTTP_ERROR_INVALID_RESPONSE = -4;

class HttpClient : public Emulator, public Client {
    public:
    HttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort = 443) {}
    HttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort = 443) {}
    HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = 443) {}
    ~HttpClient() {}

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

#endif
