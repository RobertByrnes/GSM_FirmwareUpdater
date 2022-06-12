#ifndef OAUTH2_H
#define OAUTH2_H

#include <string>

using namespace std;


class OAuth2
{
public:
    const string host;
    const string auth_path;

    OAuth2(const string& host, const string& auth_path);

    ~OAuth2();

    void setGrantType(const string &grantType, const uint clientID, const string &clientSecret);
    string getToken(string httpResponse);
    const char * personalAccessClientTokenRequestString();
    const char * postRequestString(std::string data);

private:
    uint clientId;
    string clientSecret;
    string grantType;
    string currentToken;

    string extractToken(string JSON);
    string findJson(string response);
    string tokenRequestBody();
    string to_string(uint n);
};

#endif