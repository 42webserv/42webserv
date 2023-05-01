/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:47:40 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 16:48:41 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_PARSER
#define HTTP_REQUEST_PARSER

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

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
    void printResult(const HTTPRequest &request);
};

#endif
