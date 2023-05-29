/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:15:13 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/29 14:07:13 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
#include "HTTPRequestParser.hpp"

HTTPRequestParser::HTTPRequestParser() : state_(METHOD) {}

/**
 * HTTP 요청 메세지를 파싱해서 구조체에 담아 반환
 *
 * @param data HTTP 요청 메세지가 담긴 문자열
 * @return 파싱된 HTTP 요청 구조체
 */
HTTPRequest *HTTPRequestParser::parse(const std::string &data)
{
    buffer_.clear();
    headers_.clear();
    // std::cout << "buffer_ : [" << buffer_ << "]" << std::endl;
    buffer_ += data;
    state_ = METHOD;
    std::cout << "data: [" << data << "]" << std::endl;

    while (!buffer_.empty())
    {
        switch (state_)
        {
        case METHOD:
            if (!parseMethod())
                return NULL;
            break;
        case PATH:
            if (!parsePath())
                return NULL;
            break;
        case HTTP_VERSION:
            if (!parseHTTPVersion())
                return NULL;
            break;
        case HEADER_NAME:
            if (!parseHeaderName())
                return NULL;
            break;
        case HEADER_VALUE:
            if (!parseHeaderValue())
                return NULL;
            break;
        case BODY:
            if (!parseBody())
                return NULL;
            break;
        default:
            return NULL;
        }
    }

    if (state_ == COMPLETE)
    {
        HTTPRequest *request = new HTTPRequest;
        request->method = method_;
        request->path = path_;
        request->http_version = http_version_;
        std::cout << "hereherhe" << std::endl;
        if (request->method == "HEAD")
            return request;
        // header가 존재하지 않는 경우 다시 요청 다시 받기 위함
        if (headers_.size() == 0)
            return request;
        request->headers = headers_;
        std::map<std::string, std::string>::iterator it = request->headers.find("Host");
        if (it != headers_.end())
        {
            size_t pos = it->second.find(":");
            request->port = strtod(it->second.substr(pos + 1, it->second.length()).c_str(), NULL);
            if (request->port == 0)
                request->port = -1;
            std::cout << "request->port : " << request->port << std::endl;
            request->strPort = it->second.substr(pos + 1, it->second.length());
        }
        else
            request->port = -1;
        request->body = body_;
        request->addr = addr_;
        // printResult(*request);
        reset();
        return request;
    }

    return NULL;
}

/**
 * HTTP 요청 메세지에서 METHOD 관련 파싱
 *
 * @return 올바른 METHOD라면 구조체에 저장 후 true 반환, 올바르지 않은 METHOD일 경우 false 반환
 */
bool HTTPRequestParser::parseMethod()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    std::string method_str = buffer_.substr(0, pos);
    if (method_str == "GET" || method_str == "HEAD" || method_str == "POST" || method_str == "PUT" || method_str == "PATCH" || method_str == "DELETE" || method_str == "CONNECT" || method_str == "TRACE" || method_str == "OPTIONS")
        method_ = method_str;
    else
        return false;
    state_ = PATH;
    buffer_.erase(0, pos + 1);
    return true;
}

/**
 * HTTP 요청 메세지에서 Path 관련 파싱
 *
 * @return 경로가 존재한다면 true 반환, 경로가 존재하지 않다면 false 반환
 */
bool HTTPRequestParser::parsePath()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    path_ = buffer_.substr(0, pos);

    // 만약 path_가 "/aaaa/bbbb/"이라면, 마지막 "/"를 제거해주기 위함.
    if (path_ != "/" && path_.substr(path_.length() - 1) == "/")
        path_ = buffer_.substr(0, pos - 1);
    state_ = HTTP_VERSION;
    buffer_.erase(0, pos + 1);
    pos = path_.find("?");
    if (pos != std::string::npos)
        query_ = path_.substr(pos);
    return true;
}

size_t minPos(size_t p1, size_t p2, size_t p3)
{
    return (p1 < p2 && p1 < p3 ? p1 : (p2 < p3 && p2 < p1 ? p2 : p3));
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHTTPVersion()
{
    size_t pos1 = buffer_.find("\r");
    size_t pos2 = buffer_.find("\n");
    size_t pos3 = buffer_.find(CRLF);
    if (pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
        return false;
    size_t pos = minPos(pos1, pos2, pos3);
    http_version_ = buffer_.substr(0, pos);
    state_ = HEADER_NAME;
    // 지금까지 사용한 버퍼 지우기
    buffer_.erase(0, pos);
    // 버퍼 개행이 \n, \r, \r\n 에 따라 각각 처리
    if (buffer_.find("\n") == 0)
        buffer_.erase(0, 1);
    else if (buffer_.find("\r") == 0 && buffer_.find("\n") == 1)
        buffer_.erase(0, 2);
    else if (buffer_.find("\r") == 0)
        buffer_.erase(0, 1);
    if (buffer_.empty())
    {
        std::cout << "com1" << std::endl;
        state_ = COMPLETE;
    }
    return true;
}

/**
 * HTTP 요청 메세지에서 HEADER의 이름 부분 파싱
 *
 * @return 해더의 이름이 존재한다면 현재 이름 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHeaderName()
{
    size_t pos = buffer_.find(':');
    // 만약 HTTP요청 메세지에서 헤더가 끝까지 제대로 오지 않는 경우, 그 이전 정보까지만 활용
    if (pos == std::string::npos)
    {
        std::cout << "com2" << std::endl;
        state_ = (method_ == "POST" || method_ == "PUT") ? BODY : COMPLETE;
        buffer_.clear();
        return true;
    }
    current_header_name_ = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 1);
    state_ = HEADER_VALUE;
    return true;
}

/**
 * HTTP 요청 메세지에서 HEADER의 값 파싱.
 * 만약 BODY를 받을 필요가 없는 요청이라면 COMPLETE로 상태를 변경, BODY를 받아야하는 요청이면 BODY로 상태를 변경
 *
 * @return 헤더의 값이 존재한다면 이미 저장되어있는 헤더 이름과 함께 map에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHeaderValue()
{
    size_t pos1 = buffer_.find("\r");
    size_t pos2 = buffer_.find("\n");
    size_t pos3 = buffer_.find(CRLF);
    if ((method_ != "POST" && method_ != "HEAD") && pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
    {
        // std::cout << "HEAD : " << method_ << std::endl;
        return false;
    }
    size_t pos = minPos(pos1, pos2, pos3);
    std::string header_value = buffer_.substr(1, pos);
    // std::cout << "current_header_name_ : " << current_header_name_ << ", header_value : " << header_value << std::endl;
    headers_.insert(std::make_pair(current_header_name_, header_value));
    buffer_.erase(0, pos);
    // 버퍼 개행이 \n, \r, \r\n 에 따라 각각 처리
    if (buffer_.find("\n") == 0)
        buffer_.erase(0, 1);
    else if (buffer_.find("\r") == 0 && buffer_.find("\n") == 1)
        buffer_.erase(0, 2);
    else if (buffer_.find("\r") == 0)
        buffer_.erase(0, 1);
    if (current_header_name_ == "Host")
    {
        pos = header_value.find(":");
        if (pos != std::string::npos)
            addr_ = header_value.substr(0, pos);
    }
    if (buffer_.substr(0, 2) == CRLF)
    {
        std::cout << "here" << std::endl;
        buffer_.erase(0, 2);
        body_ = "";
        state_ = (method_ == "POST" || method_ == "PUT") ? BODY : COMPLETE;
        if (state_ == COMPLETE)
            std::cout << "com3" << std::endl;
        // std::cout << "state_ :" << method_ << std::endl;
        // if (state_ == BODY && buffer_.empty())
        // {
        //     std::cout << "com4" << std::endl;
        //     state_ = COMPLETE;
        // }
    }
    else if (buffer_.empty())
    {
        state_ = (method_ == "POST" || method_ == "PUT") ? BODY : COMPLETE;
    }
    else
        state_ = HEADER_NAME;
    return true;
}

std::string sizeToString(size_t value)
{
    char buffer[20];                                     // Choose an appropriate size for the buffer
    std::memset(buffer, 0, sizeof(buffer));              // Clear the buffer
    std::snprintf(buffer, sizeof(buffer), "%zu", value); // Use snprintf to format the value as a string
    return buffer;
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseBody()
{
    if (method_ == "POST" || method_ == "PUT")
    {
        std::map<std::string, std::string>::iterator it =
            headers_.find("content-length");
        if (it != headers_.end())
        {
            int content_length = atoi(it->second.c_str());
            if (buffer_.size() < static_cast<size_t>(content_length))
                return false;
            body_ = buffer_.substr(0, content_length);
            buffer_.erase(0, content_length);
        }
        else
        {
            headers_.insert(std::make_pair("content-length", sizeToString(buffer_.length())));
            body_ = buffer_.substr(0, buffer_.size());
            buffer_.clear();
        }
    }
    if (body_ == "")
        return false;
    state_ = COMPLETE;
    return true;
}

/**
 * HTTP 요청 메세지 파싱이 끝나면 사용한 버퍼를 모두 지워줌
 */
void HTTPRequestParser::reset()
{
    state_ = METHOD;
    headers_.clear();
    body_.clear();
    buffer_.clear();
    current_header_name_.clear();
}

/**
 * HTTP 요청 메세지에서 Content-Type 헤더의 값을 반환
 *
 * @param request 파싱된 HTTP 요청
 * @return 문자열의 Content-Type 값 혹은 빈 문자열
 */
std::string HTTPRequestParser::getContentType(const HTTPRequest &request)
{
    std::map<std::string, std::string> headers = request.headers;

    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "Content-Type")
            return it->second;
    }
    return "";
}

/**
 * HTTP 요청 구조체 출력
 *
 * @param request 파싱된 HTTP 요청
 * @return void
 */
void HTTPRequestParser::printResult(const HTTPRequest &result)
{
    std::cout << "Request method: " << result.method << std::endl;
    std::cout << "Request path: " << result.path << std::endl;
    std::cout << "Request port: [" << result.port << "]" << std::endl;
    std::cout << "Request HTTP version: " << result.http_version << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = result.headers.begin(); it != result.headers.end(); ++it)
        std::cout << "Header: " << it->first << " = " << it->second << std::endl;

    std::cout << "Body: " << result.body << std::endl;
}

int HTTPRequestParser::getPort(const HTTPRequest &result)
{
    return result.port;
}
