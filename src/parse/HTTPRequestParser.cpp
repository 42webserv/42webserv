/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:15:13 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/06 22:42:40 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
#include "commonProcess.hpp"
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
    reset();
    buffer_ = data;
    chunked_data = "";
    state_ = METHOD;
    bufferIndex = 0;
    bodySize_ = -1;

    try
    {
        parseStartLine();
        parseHeaders();
        parseBody();
    }
    catch (std::exception &e)
    {
        // Parse error 말고 다른 에러 확인
        if (!(dynamic_cast<ParseException *>(&e)))
            std::cout << "Error: " << e.what() << std::endl;
        return NULL;
    }
    return makeRequest();
}

/**
 * HTTP 요청 메세지에서 METHOD 관련 파싱
 *
 * @return 올바른 METHOD라면 구조체에 저장 후 true 반환, 올바르지 않은 METHOD일 경우 false 반환
 */
bool HTTPRequestParser::parseMethod()
{
    size_t pos = buffer_.find(' ', bufferIndex);
    if (pos == std::string::npos)
        return false;
    std::string method_str = buffer_.substr(0, pos);
    if (isMethod(method_str))
        method_ = method_str;
    else
        return false;
    bufferIndex = pos + 1;
    state_ = PATH;
    return true;
}

/**
 * HTTP 요청 메세지에서 Path 관련 파싱
 *
 * @return 경로가 존재한다면 true 반환, 경로가 존재하지 않다면 false 반환
 */
bool HTTPRequestParser::parsePath()
{
    size_t pos = buffer_.find(' ', bufferIndex);
    if (pos == std::string::npos)
        return false;
    path_ = buffer_.substr(bufferIndex, pos - bufferIndex);

    // 만약 path_가 "/aaaa/bbbb/"이라면, 마지막 "/"를 제거해주기 위함.
    if (path_ != "/" && path_.substr(path_.length() - 1) == "/")
        path_ = buffer_.substr(bufferIndex, pos - bufferIndex - 1);
    bufferIndex = pos + 1;
    pos = path_.find("?");
    if (pos != std::string::npos)
        query_ = path_.substr(pos);
    state_ = HTTP_VERSION;
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
    size_t pos1 = buffer_.find("\r", bufferIndex);
    size_t pos2 = buffer_.find("\n", bufferIndex);
    size_t pos3 = buffer_.find(CRLF, bufferIndex);
    if (pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
        return false;
    size_t pos = minPos(pos1, pos2, pos3);
    http_version_ = buffer_.substr(bufferIndex, pos - bufferIndex);
    bufferIndex = pos;
    // 버퍼 개행이 \n, \r, \r\n 에 따라 각각 처리
    if (buffer_.find("\n", bufferIndex) == bufferIndex)
        bufferIndex++;
    else if (buffer_.find("\r", bufferIndex) == bufferIndex && buffer_.find("\n", bufferIndex) == bufferIndex + 1)
        bufferIndex += 2;
    else if (buffer_.find("\r", bufferIndex) == bufferIndex)
        bufferIndex++;
    state_ = HEADER_NAME;
    if (buffer_.length() == bufferIndex)
    {
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
    size_t pos = buffer_.find(':', bufferIndex);
    // 만약 HTTP요청 메세지에서 헤더가 끝까지 제대로 오지 않는 경우, 그 이전 정보까지만 활용
    if (pos == std::string::npos)
    {
        state_ = needBody(method_) ? BODY : COMPLETE;
        buffer_.clear();
        return false; // 완전한 요청만 받기
    }
    current_header_name_ = buffer_.substr(bufferIndex, pos - bufferIndex);
    bufferIndex = pos + 1;
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
    size_t pos1 = buffer_.find("\r", bufferIndex);
    size_t pos2 = buffer_.find("\n", bufferIndex);
    size_t pos3 = buffer_.find(CRLF, bufferIndex);
    if (pos1 == std::string::npos && pos2 == std::string::npos && pos3 == std::string::npos)
        return false;
    size_t pos = minPos(pos1, pos2, pos3);
    std::string header_value = buffer_.substr(bufferIndex + 1, pos - bufferIndex - 1);
    headers_.insert(std::make_pair(current_header_name_, header_value));
    bufferIndex = pos;
    // 버퍼 개행이 \n, \r, \r\n 에 따라 각각 처리
    if (buffer_.find("\n", bufferIndex) == bufferIndex)
        bufferIndex++;
    else if (buffer_.find("\r", bufferIndex) == bufferIndex && buffer_.find("\n", bufferIndex) == bufferIndex + 1)
        bufferIndex += 2;
    else if (buffer_.find("\r", bufferIndex) == bufferIndex)
        bufferIndex++;
    if (current_header_name_ == "Host")
    {
        pos = header_value.find(":");
        if (pos != std::string::npos)
        {
            addr_ = header_value.substr(0, pos);
            port_ = header_value.substr(pos + 1);
        }
    }
    if (buffer_.substr(bufferIndex, 2) == CRLF)
    {
        bufferIndex += 2;
        body_ = "";
        if (needBody(method_))
        {
            state_ = BODY;
        }
        else
        {
            buffer_.clear();
            state_ = COMPLETE;
        }
    }
    else if (buffer_.size() == bufferIndex)
        state_ = needBody(method_) ? BODY : COMPLETE;
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
 * HTTP 요청 메세지 파싱이 끝나면 사용한 버퍼를 모두 지워줌
 */
void HTTPRequestParser::reset()
{
    state_ = METHOD;
    headers_.clear();
    body_.clear();
    buffer_.clear();
    current_header_name_.clear();
    query_.clear();
    addr_.clear();
    name_.clear();
    port_ = "-1";
    path_.clear();
    http_version_.clear();
}

HTTPRequest *HTTPRequestParser::makeRequest()
{
    if (state_ != COMPLETE)
        return NULL;
    HTTPRequest *request = new HTTPRequest;
    request->method = method_;
    request->path = path_;
    request->http_version = http_version_;
    request->chunked = false;
    if (request->method == HEAD)
        return request;
    // header가 존재하지 않는 경우 다시 요청 다시 받기 위함
    if (headers_.size() == 0) // TODO 이상한 요청도 테스트 해봐야 할 듯
        return request;
    request->headers = headers_;
    request->port = ftStoi(port_);
    if (request->port < 1)
    {
        std::map<std::string, std::string>::iterator findHostIterator = request->headers.find("Host");
        if (findHostIterator != headers_.end())
        {
            size_t pos = findHostIterator->second.find(":");
            request->port = strtod(findHostIterator->second.substr(pos + 1, findHostIterator->second.length()).c_str(), NULL);
        }
    }
    request->body = body_;
    request->bodySize = bodySize_;
    request->addr = addr_;
    request->query = query_;
    std::map<std::string, std::string>::iterator findChunkedIterator = request->headers.find("Transfer-Encoding");
    if (findChunkedIterator != request->headers.end() && findChunkedIterator->second == "chunked")
        request->chunked = true;
    return request;
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

void HTTPRequestParser::parseStartLine()
{
    bool isOK;
    size_t pos = buffer_.find(CRLF);

    if (pos == std::string::npos)
        throw ParseException();
    while (bufferIndex < pos)
    {
        isOK = false;
        switch (state_)
        {
        case METHOD:
            isOK = parseMethod();
            break;
        case PATH:
            isOK = parsePath();
            break;
        case HTTP_VERSION:
            isOK = parseHTTPVersion();
            break;
        default:
            break;
        }
        if (!isOK)
            throw ParseException();
    }
}
void HTTPRequestParser::parseHeaders()
{
    bool isOK;
    size_t pos = buffer_.find(CRLF2);

    if (pos == std::string::npos)
        throw ParseException();
    while (bufferIndex < pos)
    {
        isOK = false;
        switch (state_)
        {
        case HEADER_NAME:
            isOK = parseHeaderName();
            break;
        case HEADER_VALUE:
            isOK = parseHeaderValue();
            break;
        default:
            break;
        }
        if (!isOK)
            throw ParseException();
    }
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseBody()
{
    if (state_ != BODY)
        return false;
    std::map<std::string, std::string>::iterator it = headers_.find("Transfer-Encoding");
    if (it != headers_.end() && it->second == "chunked")
    {
        // chunked 인코딩이 적용된 경우
        while (buffer_.size() != bufferIndex)
        {
            size_t pos = buffer_.find(CRLF, bufferIndex); // 청크의 크기를 나타내는 줄바꿈 위치 찾기
            if (pos == std::string::npos)
                return false;
            std::string chunk_size_str = buffer_.substr(bufferIndex, pos - bufferIndex); // 청크의 크기를 나타내는 문자열
            bufferIndex = pos + 2;

            if (chunk_size_str.empty())
                break; // 마지막 청크를 나타내는 빈 문자열인 경우 종료

            if (buffer_.length() > 2 && buffer_.substr(buffer_.length() - 6, 2) != "\n0")
                return false;

            // chunk_size_str을 숫자로 변환
            std::istringstream iss(chunk_size_str);
            size_t chunk_size;
            if (!(iss >> std::hex >> chunk_size))
                return false;

            if (chunk_size == 0)
            {
                buffer_.clear();
                state_ = COMPLETE;
                bodySize_ = body_.length();
                headers_.insert(std::make_pair("content-length", sizeToString(bodySize_)));
                return true;
            }

            if (buffer_.size() < chunk_size + 2)
                return false;

            std::string chunk_data = buffer_.substr(bufferIndex, chunk_size); // 청크의 데이터 추출
            bufferIndex += chunk_size + 2;
            body_ += chunk_data; // body에 청크 데이터 추가

            if (buffer_.size() == 0)
                return false;
        }
    }
    else
    {
        // chunked 인코딩이 적용되지 않은 경우
        std::map<std::string, std::string>::iterator content_length_it = headers_.find("content-length");
        if (content_length_it != headers_.end())
        {
            int content_length = ftStoi(content_length_it->second);
            if (buffer_.empty())
            {
                state_ = COMPLETE;
                return true;
            }
            if (buffer_.size() < static_cast<size_t>(content_length))
                return false;
            body_ = buffer_.substr(bufferIndex, content_length);
            bufferIndex += content_length;
            bodySize_ = content_length;
        }
        else
        {
            body_ = buffer_.substr(bufferIndex, buffer_.size());
            bodySize_ = body_.length();
            headers_.insert(std::make_pair("content-length", sizeToString(bodySize_)));
            buffer_.clear();
            state_ = COMPLETE;
            return true;
        }
    }
    if (body_.empty())
        return false;
    state_ = COMPLETE;
    return true;
}

const char *HTTPRequestParser::ParseException::what() const throw()
{
    return "HTTP Request Parse Exception";
}
