#include "OAuth2.h"
#include <sstream>
#include <string.h>


// Constructor
OAuth2::OAuth2(const string& host, const string& auth_path): host(host), auth_path(auth_path), clientId(0) {};

// Destructor
OAuth2::~OAuth2() {};

/**
 * @brief Extract Auth token from JSON string.
 * 
 * @param JSON std::string
 * 
 * @return std::string 
 */
string OAuth2::extractToken(string JSON) // private
{
  int beginningOfToken = JSON.find_last_of(":") + 2;
  int endOfToken = JSON.find_last_of("\"}") - 1;
 
  string accessToken = JSON.substr(beginningOfToken, endOfToken - beginningOfToken);

  if (accessToken.length() > 1)
    return accessToken;
  else
    return string("");
}

/**
 * @brief Extract JSON from server response
 * 
 * @param response std::string 
 * 
 * @return std::string 
 */
string OAuth2::findJson(string response) // private
{
  int firstCurly = response.find_first_of("{") + 1;
  int secondCurly = response.find_first_of("}");
  string JSON = response.substr(firstCurly, secondCurly);
  
  if (JSON.length() > 1)
    return JSON;
  else
    return string("");
}

/**
 * @brief Extract Auth token from the HTTP response returned from the token endpoint.
 * 
 * @param httpResponse std::string
 * 
 * @return std::string (either the token or an empty string)
 */
string OAuth2::getToken(string httpResponse) // public
{
    string parsedResponse = this->findJson(httpResponse);

    if (parsedResponse != "") {
      this->currentToken = this->extractToken(parsedResponse);
      return this->currentToken;
    } else {
      string notFound = "";
      return notFound;
    }
}

/**
 * @brief get the request body for the request to the token endpoint.
 * 
 * @return const char *
 */
const char * OAuth2::personalAccessClientTokenRequestString() // public
{
  //** FORMAT **//
  // POST /oauth/token HTTP/1.1
  // Host: draperbiotech.clystnet.com
  // Accept: application/json
  // Content-Type: application/x-www-form-urlencoded
  // Content-Length: 96

  // grant_type=client_credentials&client_id=3&client_secret=jGDjmHMOGNfuClU9oFVCHpFxv0dbeLmOu1i3wkW2

  string requestBody = this->tokenRequestBody();
  string bodyLength = this->to_string(static_cast<uint>(requestBody.length()));
  string authRequest = "POST " + this->auth_path + " HTTP/1.1\r\n";
  authRequest.append("Host: " + this->host + "\r\n");
  authRequest.append("Accept: application/json\r\n");
  authRequest.append("Content-Type: application/x-www-form-urlencoded\r\n");
  authRequest.append("Content-Length: " + bodyLength + "\r\n\r\n");
  authRequest.append(requestBody);

  return authRequest.c_str();
}

const char * OAuth2::postRequestString(std::string data)
{
  //** FORMAT **//
  // POST /oauth/token HTTP/1.1
  // Host: draperbiotech.clystnet.com
  // Authorization: Bearer ehfu4y37hf7340hqq78048tn3y4898n758943n5789n2457v8934thgjurwegojirehgr8939u843....
  // Accept: application/json
  // Content-Type: application/x-www-form-urlencoded
  // Content-Length: 96

  string bodyLength = this->to_string(static_cast<uint>(data.length()));
  string postRequest = "POST " + this->auth_path + " HTTP/1.1\r\n";
  postRequest.append("Host: " + this->host + "\r\n");
  postRequest.append("Host: " + this->host + "\r\n");
  postRequest.append("Authorization: Bearer " + this->currentToken + "\r\n");
  postRequest.append("Accept: application/json\r\n");
  postRequest.append("Content-Type: application/json\r\n");
  postRequest.append("Content-Length: " + bodyLength + "\r\n\r\n");
  postRequest.append(data);

  return postRequest.c_str();

// String data = ("{\"udid\":\"" + String(udid) + "\", \"reading\":\"" + reading + "\", \"temp\":\"" + temp + "\", \"humid\":\"" + humid + "\", \"auger1\":\"" + augerOneCount + "\", \"auger2\":\"" + augerTwoCount + "\", \"faultCount\":\"" + faultLineCount +"\", \"noFaultCount\":\"" + noFaultCount + "\"}");
}

/**
 * @brief Set the credentials to be used for getting tokens.
 * 
 * @param grantType std::string
 * @param clientId uint
 * @param clientSecret std::string
 * 
 * @return void
 */
void OAuth2::setGrantType(const string &grantType, const uint clientId, const string &clientSecret) // public
{
  this->grantType = grantType;
  this->clientId = clientId;
  this->clientSecret = clientSecret;
}

/**
 * @brief returns a string of the request body for requesting an auth token from the server.
 *
 * @return std::string
 */
string OAuth2::tokenRequestBody() // private
{
  string requestBody = "grant_type=" + this->grantType + "&client_id=" + this->to_string(this->clientId) + "&client_secret=" + this->clientSecret;
  return requestBody;
}

/**
 * @brief Helper for converting uint data type to a string.
 * 
 * @param n uint
 * 
 * @return std::string
 */
string OAuth2::to_string(uint n) // private
{
    ostringstream s;
    s << n;
    return s.str();
}
