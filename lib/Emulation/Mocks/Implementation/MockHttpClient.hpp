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
    /** Start a more complex request.
        Use this when you need to send additional headers in the request,
        but you will also need to call endRequest() when you are finished.
    */
    void beginRequest() {}

    /** End a more complex request.
        Use this when you need to have sent additional headers in the request,
        but you will also need to call beginRequest() at the start.
    */
    void endRequest() {}

    /** Start the body of a more complex request.
        Use this when you need to send the body after additional headers
        in the request, but can optionally call endRequest() when
        you are finished.
    */
    void beginBody() {}

    /** Connect to the server and start to send a GET request.
      @param aURLPath     Url to request
      @return 0 if successful, else error
    */
    int get(const char* aURLPath) { return this->mock<int>("get"); }
    int get(const String& aURLPath) { return this->mock<int>("get"); }

    /** Connect to the server and start to send a POST request.
      @param aURLPath     Url to request
      @return 0 if successful, else error
    */
    int post(const char* aURLPath) { return this->mock<int>("post"); }
    int post(const String& aURLPath) { return this->mock<int>("post"); }

    /** Connect to the server and send a POST request
        with body and content type
      @param aURLPath     Url to request
      @param aContentType Content type of request body
      @param aBody        Body of the request
      @return 0 if successful, else error
    */
    int post(const char* aURLPath, const char* aContentType, const char* aBody) { return this->mock<int>("post"); }
    int post(const String& aURLPath, const String& aContentType, const String& aBody) { return this->mock<int>("post"); }
    int post(const char* aURLPath, const char* aContentType, int aContentLength, const uint8_t aBody[]) { return this->mock<int>("post"); }

    /** Connect to the server and start to send a PUT request.
      @param aURLPath     Url to request
      @return 0 if successful, else error
    */
    int put(const char* aURLPath) { return this->mock<int>("put"); }
    int put(const String& aURLPath) { return this->mock<int>("put"); }

    /** Connect to the server and send a PUT request
        with body and content type
      @param aURLPath     Url to request
      @param aContentType Content type of request body
      @param aBody        Body of the request
      @return 0 if successful, else error
    */
    int put(const char* aURLPath, const char* aContentType, const char* aBody) { return this->mock<int>("put"); }
    int put(const String& aURLPath, const String& aContentType, const String& aBody) { return this->mock<int>("put"); }
    int put(const char* aURLPath, const char* aContentType, int aContentLength, const uint8_t aBody[]) { return this->mock<int>("put"); }

    /** Connect to the server and start to send a PATCH request.
      @param aURLPath     Url to request
      @return 0 if successful, else error
    */
    int patch(const char* aURLPath) { return this->mock<int>("patch"); }
    int patch(const String& aURLPath) { return this->mock<int>("patch"); }

    /** Connect to the server and send a PATCH request
        with body and content type
      @param aURLPath     Url to request
      @param aContentType Content type of request body
      @param aBody        Body of the request
      @return 0 if successful, else error
    */
    int patch(const char* aURLPath, const char* aContentType, const char* aBody) { return this->mock<int>("patch"); }
    int patch(const String& aURLPath, const String& aContentType, const String& aBody) { return this->mock<int>("patch"); }
    int patch(const char* aURLPath, const char* aContentType, int aContentLength, const uint8_t aBody[]) { return this->mock<int>("patch"); }

    /** Connect to the server and start to send a DELETE request.
      @param aURLPath     Url to request
      @return 0 if successful, else error
    */
    int del(const char* aURLPath) { return this->mock<int>("del"); }
    int del(const String& aURLPath) { return this->mock<int>("del"); }

    /** Connect to the server and send a DELETE request
        with body and content type
      @param aURLPath     Url to request
      @param aContentType Content type of request body
      @param aBody        Body of the request
      @return 0 if successful, else error
    */
    int del(const char* aURLPath, const char* aContentType, const char* aBody) { return this->mock<int>("del"); }
    int del(const String& aURLPath, const String& aContentType, const String& aBody) { return this->mock<int>("del"); }
    int del(const char* aURLPath, const char* aContentType, int aContentLength, const uint8_t aBody[]) { return this->mock<int>("del"); }

    /** Connect to the server and start to send the request.
        If a body is provided, the entire request (including headers and body) will be sent
      @param aURLPath        Url to request
      @param aHttpMethod     Type of HTTP request to make, e.g. "GET", "POST", etc.
      @param aContentType    Content type of request body (optional)
      @param aContentLength  Length of request body (optional)
      @param aBody           Body of request (optional)
      @return 0 if successful, else error
    */
    int startRequest(const char* aURLPath,
                     const char* aHttpMethod,
                     const char* aContentType = NULL,
                     int aContentLength = -1,
                     const uint8_t aBody[] = NULL) { return this->mock<int>("del"); }

    /** Send an additional header line.  This can only be called in between the
      calls to beginRequest and endRequest.
      @param aHeader Header line to send, in its entirety (but without the
                     trailing CRLF.  E.g. "Authorization: Basic YQDDCAIGES"
    */
    void sendHeader(const char* aHeader) {}

    void sendHeader(const String& aHeader)
      { sendHeader(aHeader.c_str()); }

    /** Send an additional header line.  This is an alternate form of
      sendHeader() which takes the header name and content as separate strings.
      The call will add the ": " to separate the header, so for example, to
      send a XXXXXX header call sendHeader("XXXXX", "Something")
      @param aHeaderName Type of header being sent
      @param aHeaderValue Value for that header
    */
    void sendHeader(const char* aHeaderName, const char* aHeaderValue) {}

    void sendHeader(const String& aHeaderName, const String& aHeaderValue)
      { sendHeader(aHeaderName.c_str(), aHeaderValue.c_str()); }

    /** Send an additional header line.  This is an alternate form of
      sendHeader() which takes the header name and content separately but where
      the value is provided as an integer.
      The call will add the ": " to separate the header, so for example, to
      send a XXXXXX header call sendHeader("XXXXX", 123)
      @param aHeaderName Type of header being sent
      @param aHeaderValue Value for that header
    */
    void sendHeader(const char* aHeaderName, const int aHeaderValue) {}

    void sendHeader(const String& aHeaderName, const int aHeaderValue)
      { sendHeader(aHeaderName.c_str(), aHeaderValue); }

    /** Send a basic authentication header.  This will encode the given username
      and password, and send them in suitable header line for doing Basic
      Authentication.
      @param aUser Username for the authorization
      @param aPassword Password for the user aUser
    */
    void sendBasicAuth(const char* aUser, const char* aPassword) {}

    void sendBasicAuth(const String& aUser, const String& aPassword)
      { sendBasicAuth(aUser.c_str(), aPassword.c_str()); }

    /** Get the HTTP status code contained in the response.
      For example, 200 for successful request, 404 for file not found, etc.
    */
    int responseStatusCode() { return this->mock<int>("responseStatusCode"); }

    /** Check if a header is available to be read.
      Use readHeaderName() to read header name, and readHeaderValue() to
      read the header value
      MUST be called after responseStatusCode() and before contentLength()
    */
    bool headerAvailable() { return this->mock<bool>("headerAvailable"); }

    /** Read the name of the current response header.
      Returns empty string if a header is not available.
    */
    String readHeaderName() { return this->mock<String>("readHeaderName"); }

    /** Read the vallue of the current response header.
      Returns empty string if a header is not available.
    */
    String readHeaderValue() { return this->mock<String>("readHeaderValue"); }

    /** Read the next character of the response headers.
      This functions in the same way as read() but to be used when reading
      through the headers.  Check whether or not the end of the headers has
      been reached by calling endOfHeadersReached(), although after that point
      this will still return data as read() would, but slightly less efficiently
      MUST be called after responseStatusCode() and before contentLength()
      @return The next character of the response headers
    */
    int readHeader() { return this->mock<int>("readHeader"); }

    /** Skip any response headers to get to the body.
      Use this if you don't want to do any special processing of the headers
      returned in the response.  You can also use it after you've found all of
      the headers you're interested in, and just want to get on with processing
      the body.
      MUST be called after responseStatusCode()
      @return HTTP_SUCCESS if successful, else an error code
    */
    int skipResponseHeaders() { return this->mock<int>("skipResponseHeaders"); }

    /** Test whether all of the response headers have been consumed.
      @return true if we are now processing the response body, else false
    */
    bool endOfHeadersReached() { return this->mock<int>("endOfHeadersReached"); }

    /** Test whether the end of the body has been reached.
      Only works if the Content-Length header was returned by the server
      @return true if we are now at the end of the body, else false
    */
    bool endOfBodyReached() { return this->mock<int>("endOfBodyReached"); }
    virtual bool endOfStream() { return endOfBodyReached(); };
    virtual bool completed() { return endOfBodyReached(); };

    /** Return the length of the body.
      Also skips response headers if they have not been read already
      MUST be called after responseStatusCode()
      @return Length of the body, in bytes, or kNoContentLengthHeader if no
      Content-Length header was returned by the server
    */
    int contentLength() { return this->mock<int>("contentLength"); }

    /** Returns if the response body is chunked
      @return true if response body is chunked, false otherwise
    */
    int isResponseChunked() { return iIsChunked; }

    /** Return the response body as a String
      Also skips response headers if they have not been read already
      MUST be called after responseStatusCode()
      @return response body of request as a String
    */
    String responseBody() { return this->mock<String>("responseBody"); }

    /** Enables connection keep-alive mode
    */
    void connectionKeepAlive() {}

    /** Disables sending the default request headers (Host and User Agent)
    */
    void noDefaultRequestHeaders() {}

    // Inherited from Print
    // Note: 1st call to these indicates the user is sending the body, so if need
    // Note: be we should finish the header first
    // virtual size_t write(uint8_t aByte) { if (iState < eRequestSent) { finishHeaders(); }; return iClient-> write(aByte); };
    // virtual size_t write(const uint8_t *aBuffer, size_t aSize) { if (iState < eRequestSent) { finishHeaders(); }; return iClient->write(aBuffer, aSize); };
    // Inherited from Stream
    // virtual int available() { return this->mock<int>("available"); }
    /** Read the next byte from the server.
      @return Byte read or -1 if there are no bytes available.
    */
    // virtual int read() { return this->mock<int>("read"); }
    // virtual int read(uint8_t *buf, size_t size) { return this->mock<int>("read"); }
    // virtual int peek() { return iClient->peek(); }
    // virtual void flush() { iClient->flush(); }

    // Inherited from Client
    // virtual int connect(IPAddress ip, uint16_t port) { return iClient->connect(ip, port); }
    // virtual int connect(const char *host, uint16_t port) { return iClient->connect(host, port); }
    // virtual void stop() {}
    // virtual uint8_t connected() { return iClient->connected(); };
    // virtual operator bool() { return bool(iClient); };
    // virtual uint32_t httpResponseTimeout() { return iHttpResponseTimeout; };
    // virtual void setHttpResponseTimeout(uint32_t timeout) { iHttpResponseTimeout = timeout; };
protected:
    /** Reset internal state data back to the "just initialised" state
    */
    void resetState() {}

    /** Send the first part of the request and the initial headers.
      @param aURLPath	Url to request
      @param aHttpMethod  Type of HTTP request to make, e.g. "GET", "POST", etc.
      @return 0 if successful, else error
    */
    int sendInitialHeaders(const char* aURLPath, const char* aHttpMethod) { return this->mock<int>("sendInitialHeaders"); }

    /* Let the server know that we've reached the end of the headers
    */
    void finishHeaders() {}

    /** Reading any pending data from the client (used in connection keep alive mode)
    */
    void flushClientRx() {}

    // Number of milliseconds that we wait each time there isn't any data
    // available to be read (during status code and header processing)
    static const int kHttpWaitForDataDelay = 1000;
    // Number of milliseconds that we'll wait in total without receiveing any
    // data before returning HTTP_ERROR_TIMED_OUT (during status code and header
    // processing)
    static const int kHttpResponseTimeout = 30*1000;
    static const char* kContentLengthPrefix;
    static const char* kTransferEncodingChunked;
    typedef enum {
        eIdle,
        eRequestStarted,
        eRequestSent,
        eReadingStatusCode,
        eStatusCodeRead,
        eReadingContentLength,
        eSkipToEndOfHeader,
        eLineStartingCRFound,
        eReadingBody,
        eReadingChunkLength,
        eReadingBodyChunk
    } tHttpState;
    // Client we're using
    Client* iClient;
    // Server we are connecting to
    const char* iServerName;
    IPAddress iServerAddress;
    // Port of server we are connecting to
    uint16_t iServerPort;
    // Current state of the finite-state-machine
    tHttpState iState;
    // Stores the status code for the response, once known
    int iStatusCode;
    // Stores the value of the Content-Length header, if present
    int iContentLength;
    // How many bytes of the response body have been read by the user
    int iBodyLengthConsumed;
    // How far through a Content-Length header prefix we are
    const char* iContentLengthPtr;
    // How far through a Transfer-Encoding chunked header we are
    const char* iTransferEncodingChunkedPtr;
    // Stores if the response body is chunked
    bool iIsChunked;
    // Stores the value of the current chunk length, if present
    int iChunkLength;
    uint32_t iHttpResponseTimeout;
    bool iConnectionClose;
    bool iSendDefaultRequestHeaders;
    String iHeaderLine;
};

#endif
