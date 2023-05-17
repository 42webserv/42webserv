/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:47:40 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 17:18:27 by chanwjeo         ###   ########.fr       */
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
    std::string http_version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string buffer_;
    std::string current_header_name_;
    std::string query_;
    std::string addr_;
    std::string name_;
    std::string port_;

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
    std::string getContentType(const HTTPRequest &request);
    int getPort(const HTTPRequest &result);
    void printResult(const HTTPRequest &request);
};

#endif
