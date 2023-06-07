/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:16:36 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/07 15:23:40 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

/*
 * A destructor
 */
Utils::~Utils() {}

/**
 * 3개의 인자 중 가장 작은 값 반환
 *
 * @param size_t 3개의 양수
 */
size_t Utils::minPos(size_t p1, size_t p2, size_t p3)
{
    return (p1 < p2 && p1 < p3 ? p1 : (p2 < p3 && p2 < p1 ? p2 : p3));
}

/**
 * 10진수 인자를 16진수 문자열로 반환
 *
 * @param size_t 10진수 값
 * @return 16진수 문자열
 */
std::string Utils::toHexString(size_t value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}

/**
 * 문자열로 이루어진 수를 int형 정수로 반환
 *
 * @param str 문자열
 * @return int 형 정수
 */
int Utils::ftStoi(const std::string &str)
{
    std::stringstream ss(str);
    int requestBodySize;
    ss >> requestBodySize;
    return requestBodySize;
}

/**
 * 에러 코드에 대한 페이지가 존재하지 않는 경우 페이지 새로 생성
 *
 * @param response 응답을 위한 구조체
 * @param client_fd 브라우저 포트번호
 * @return html로 구성된 문자열
 */
std::string Utils::errorPageGenerator(ResponseData *response, int errorCode)
{
    std::stringstream broadHtml;
    broadHtml << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"utf-8\">\n\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n\t<metaname=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>error page</title>\n</head>\n<body>\n\t<h1>" << errorCode << " " << response->statusCodeMap[errorCode] << ".</h1>\n</body>\n</html>";
    return broadHtml.str();
}

void Utils::setTimer(const int kq, int fd, int timeout)
{
    struct kevent timerEvent;
    int timer_interval_ms = timeout * 1000;
    EV_SET(&timerEvent, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, timer_interval_ms, 0);
    kevent(kq, &timerEvent, 1, NULL, 0, NULL);
}

void Utils::deleteTimer(const int kq, int fd)
{
    struct kevent timerEvent;
    EV_SET(&timerEvent, fd, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
    kevent(kq, &timerEvent, 1, NULL, 0, NULL);
}

std::string Utils::getExpiryDate(int secondsToAdd)
{
    std::time_t now = std::time(0);
    std::tm *expiration = std::localtime(&now);
    expiration->tm_sec += secondsToAdd;
    std::mktime(expiration);
    char buffer[80];
    std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", expiration);
    return std::string(buffer);
}
bool Utils::needBody(const std::string &method)
{
    if (method.empty())
        return false;
    if (isEqual(method, POST) || isEqual(method, PUT))
        return true;
    return false;
}

std::string Utils::extractSubstring(const std::string &A, const std::string &B, const std::string &C)
{
	size_t start = A.find(B);
	if (start == std::string::npos)
		return "";
	start += B.length();
	size_t end = A.find(C, start);
	if (C == "\0")
		return A.substr(start);
	if (end == std::string::npos)
		return "";

	return A.substr(start, end - start);
}
