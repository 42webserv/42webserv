#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum HTTPMethod
{
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DELETE,
    CONNECT,
    TRACE,
    OPTIONS
};

struct HTTPRequest
{
    HTTPMethod method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
};

class HTTPRequestParser
{
private:
    enum ParseState
    {
        METHOD,
        PATH,
        HTTP_VERSION,
        HEADER_NAME,
        HEADER_VALUE,
        BODY,
        COMPLETE
    };

    ParseState state_;
    HTTPMethod method_;
    std::string path_;
    std::string http_version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string buffer_;
    std::string current_header_name_;

    bool parseMethod();
    bool parsePath();
    bool parseHttpVersion();
    bool parseHeaderName();
    bool parseHeaderValue();
    bool parseBody();
    void reset();

public:
    HTTPRequestParser();

    HTTPRequest *parse(const std::string &data);
};
