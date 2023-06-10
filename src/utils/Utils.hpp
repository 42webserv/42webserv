/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:15:56 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/10 19:47:08 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Utils_HPP
#define Utils_HPP

#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include "Response.hpp"

#define GET "GET"
#define HEAD "HEAD"
#define POST "POST"
#define PUT "PUT"
#define PATCH "PATCH"
#define DELETE "DELETE"
#define CONNECT "CONNECT"
#define TRACE "TRACE"
#define OPTIONS "OPTIONS"

struct ResponseData;

class Utils
{
private:
    Utils();

public:
    /*
     * A default constructor
     */

    /*
     * A destructor
     */
    ~Utils();

    /*
     * Add it if you feel necessary additional member functions.
     */
    static size_t minPos(size_t p1, size_t p2, size_t p3);
    static std::string toHexString(size_t value);
    static std::string errorPageGenerator(ResponseData *response, int errorCode);
    static void setTimer(const int &kq, const int &fd, const int &timeout);
    static std::string getExpiryDate(int secondsToAdd);
    static bool needBody(const std::string &method);
    static std::string extractSubstring(const std::string &A, const std::string &B, const std::string &C);
    static std::string uploadPageGenerator(std::string executePath);
    static std::vector<Directive>::const_iterator findDirective(const std::vector<Directive> &directives, const std::string &name);
    static std::vector<Directive>::const_iterator findDirectiveNameValue(const std::vector<Directive> &directives, const std::string &name, const std::string &value);
    static const std::string getContentType(const HTTPRequest &request);
    static std::string lower(const std::string &s);
    static bool isEqual(const std::string &s1, const std::string &s2);
    static bool writeFile(const std::string &path, const std::string &contents);
    static std::string readFile(const std::string &path);
    static bool isDirectory(const std::string &path);
    static bool isFile(const std::string &path);
    static bool isMethod(const std::string &method);
    static std::string getLastStringSplit(std::string &str, const char *sep);

    static void ftSend(const int &socket, const std::string &buffer);
    static void ftSend(const ResponseData &response, const std::string &contents);
    static void ftSend(const ResponseData *response, const std::string &contents);

    static int ftStoi(const std::string &str);

    static std::string getTime();

    template <typename T>
    static std::string ftToString(T value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
};

#endif // Utils_HPP
