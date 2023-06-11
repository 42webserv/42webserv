/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:47:40 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/11 22:42:24 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "HTTPRequest.hpp"

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
    std::string port_;
    unsigned long long bodySize_;

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
    // Start line
    std::string getMethod() const;
    std::string getPath() const;
    std::string getQuery() const;
    std::string getHttpVersion() const;
    // Headers
    std::map<std::string, std::string> getHeaders() const;
    // Body
    std::string getBody() const;
};

#endif
