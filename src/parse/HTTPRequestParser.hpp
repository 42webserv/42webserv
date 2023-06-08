/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:47:40 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/08 18:39:06 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct HTTPRequest
{
    std::string method;
    int port;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string query;
    std::string addr;
    std::string name;
    bool chunked;
    long long bodySize;
    unsigned int statusCode;
    //  SERVER_NAME 요청을 수신한 서버의 호스트 이름. -> conf에서 가져올것
    HTTPRequest &operator=(const HTTPRequest &ref);
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
    std::string method_;
    std::string path_;
    std::string httpVersion_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string chunkedData; // chunked 인코딩된 데이터를 저장할 필드
    std::string buffer_;
    size_t bufferIndex;
    std::string currentHeaderName_;
    std::string query_;
    std::string addr_;
    std::string name_;
    std::string port_;
    long long bodySize_;
    unsigned int statusCode_;

    bool parseMethod();
    bool parsePath();
    bool parseHTTPVersion();
    bool parseHeaderName();
    bool parseHeaderValue();
    void reset();
    HTTPRequest *makeRequest();
    void parseStartLine();
    void parseHeaders();
    bool parseBody();
    void checkHeaders();
    class ParseException : public std::exception
    {
    public:
        const char *what() const throw();
    };

public:
    HTTPRequestParser();

    HTTPRequest *parse(const std::string &data);
    void printResult(const HTTPRequest &request);
};

#endif
