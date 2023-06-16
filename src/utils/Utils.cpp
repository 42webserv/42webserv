/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:16:36 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/12 18:13:38 by sanghan          ###   ########.fr       */
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

void Utils::setTimer(const int &kq, const int &fd, const int &timeout)
{
    struct kevent timerEvent;
    int timer_interval_ms = timeout * 1000;
    EV_SET(&timerEvent, fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, timer_interval_ms, 0);
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

/**
 *
 *
 * @param client_fd 브라우저 포트번호
 */
std::string Utils::uploadPageGenerator(std::string executePath)
{
    std::stringstream broadHtml;
    broadHtml << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"utf-8\">\n\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n\t<metaname=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>Upload</title>\n</head>\n<body>\n\t<form action=\"" << executePath << "\" method=\"post\" enctype=\"multipart/form-data\">\n\t<p><input type=\"file\" name=\"file1\"></p>\n\t<p><button type=\"submit\">Submit</button></p>\n\t</form>\n</body>\n</html>";
    return broadHtml.str();
}

std::vector<Directive>::const_iterator Utils::findDirective(const std::vector<Directive> &directives, const std::string &name)
{
    for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
    {
        if (isEqual(it->name, name))
            return it;
    }
    return directives.end();
}

std::vector<Directive>::const_iterator Utils::findDirectiveNameValue(const std::vector<Directive> &directives, const std::string &name, const std::string &value)
{
    for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
    {
        if (isEqual(it->name, name) && isEqual(it->value, value))
            return it;
    }
    return directives.end();
}

/**
 * HTTP 요청 메세지에서 Content-Type 헤더의 값을 반환
 *
 * @param request 파싱된 HTTP 요청
 * @return 문자열의 Content-Type 값 혹은 기본값
 */
const std::string Utils::getContentType(const HTTPRequest &request)
{
    const std::map<std::string, std::string> &headers = request.headers;

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (Utils::isEqual(it->first, "Content-Type"))
            return it->second;
    }
    return "text/plain";
}

bool Utils::needBody(const std::string &method)
{
    if (method.empty())
        return false;
    if (Utils::isEqual(method, POST) || Utils::isEqual(method, PUT))
        return true;
    return false;
}

std::string Utils::lower(const std::string &s)
{
    std::string lowerS = s;

    for (size_t i = 0; i < lowerS.size(); i++)
    {
        lowerS[i] = std::tolower(lowerS[i]);
    }
    return lowerS;
}

bool Utils::isEqual(const std::string &s1, const std::string &s2)
{
    std::string lowerS1 = lower(s1);
    std::string lowerS2 = lower(s2);

    return lowerS1 == lowerS2;
}

bool Utils::writeFile(const std::string &path, const std::string &contents)
{
    std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);

    if (!file.is_open())
        return false;
    file << contents;
    file.close();
    return true;
}

std::string Utils::readFile(const std::string &path)
{
    std::ifstream file(path.c_str());

    if (!file.is_open())
        return "";
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return contents;
}

/**
 * @brief Check if the path is a directory
 * @param path
 */
bool Utils::isDirectory(const std::string &path)
{
    struct stat st;

    stat(path.c_str(), &st);
    if (S_ISDIR(st.st_mode))
        return true;
    return false;
}

/**
 * @brief Check if the path is a file
 * @param path
 */
bool Utils::isFile(const std::string &path)
{
    struct stat st;

    stat(path.c_str(), &st);
    if (S_ISREG(st.st_mode))
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

/**
 * @brief Send response to client
 * @param socket
 * @param buffer
 */
void Utils::ftSend(const int &socket, const std::string &buffer)
{
    if (buffer.empty())
        return;
    ssize_t sendSize = send(socket, buffer.c_str(), buffer.length(), 0);
    if (sendSize != (ssize_t)buffer.length())
    {
        ftSend(socket, buffer);
        sendSize = (ssize_t)buffer.length();
    }
}

/**
 * @brief Send response to client
 * @param response
 * @param contents
 */
void Utils::ftSend(const ResponseData *response, const std::string &contents)
{
    Utils::ftSend(response->clientFd, contents);
}

void Utils::ftSend(const ResponseData &response, const std::string &contents)
{
    Utils::ftSend(response.clientFd, contents);
}

std::string Utils::getLastStringSplit(std::string &str, const char *sep)
{
    size_t pos = str.rfind(sep);
    if (pos != std::string::npos)
        return str.substr(pos + 1);
    return "";
}

/**
 * @brief Get current time
 */
std::string Utils::getTime()
{
    std::time_t currentTime = std::time(nullptr); // 현재 시간 가져오기

    // tm 구조체로 시간 정보 얻기
    std::tm *timeInfo = std::localtime(&currentTime);

    // 원하는 형식으로 시간 값을 포맷팅
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%d/%b/%Y:%H:%M:%S %z", timeInfo);
    return buffer;
}
