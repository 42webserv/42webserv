/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:15:56 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/07 15:06:34 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Utils_HPP
#define Utils_HPP

#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include "Response.hpp"

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
    static void setTimer(const int kq, int fd, int timeout);
    static void deleteTimer(const int kq, int fd);
    static std::string getExpiryDate(int secondsToAdd);
    static bool needBody(const std::string &method);

    static int ftStoi(const std::string &str);
    template <typename T>
    static std::string ftToString(T value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
};

#endif // Utils_HPP
