/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:15:13 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/11 20:49:04 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
#include "Utils.hpp"
#include "HTTPRequestParser.hpp"

#define TRANSFER_ENCODING "Transfer-Encoding"
#define CONTENT_LENGTH "Content-Length"
#define CHUNKED "chunked"
#define HOST "Host"

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
    if (Utils::isMethod(method_str))
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
    bufferIndex = pos + 1;
    pos = path_.find(QUESTION);
    if (pos != std::string::npos)
    {
        query_ = path_.substr(pos);
        path_.erase(pos);
    }
    state_ = HTTP_VERSION;
    return true;
}

/**
 * HTTP 요청 메세지에서 HTTP 버전 파싱
 *
 * @return HTTP 버전이 존재한다면 구조체에 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHTTPVersion()
{
    size_t pos = buffer_.find(CRLF, bufferIndex);
    if (pos == std::string::npos)
        return false;
    httpVersion_ = buffer_.substr(bufferIndex, pos - bufferIndex);
    bufferIndex = pos + 2;
    state_ = HEADER_NAME;
    return true;
}

/**
 * HTTP 요청 메세지에서 HEADER의 이름 부분 파싱
 *
 * @return 해더의 이름이 존재한다면 현재 이름 저장 후 true 반환, 존재하지 않는다면 false 반환
 */
bool HTTPRequestParser::parseHeaderName()
{
    size_t pos = buffer_.find(CRLF, bufferIndex);
    if (pos == std::string::npos)
        return false;
    pos = buffer_.find(COLON, bufferIndex);
    if (pos == std::string::npos)
        return false;
    currentHeaderName_ = buffer_.substr(bufferIndex, pos - bufferIndex);
    if (Utils::isEqual(currentHeaderName_, CONTENT_LENGTH))
        currentHeaderName_ = CONTENT_LENGTH;
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
    size_t pos = buffer_.find(CRLF, bufferIndex);
    if (pos == std::string::npos)
        return false;
    std::string header_value = buffer_.substr(bufferIndex + 1, pos - bufferIndex - 1);
    headers_.insert(std::make_pair(currentHeaderName_, header_value));
    bufferIndex = pos + 2;
    if (currentHeaderName_ == HOST)
    {
        pos = header_value.find(COLON);
        if (pos != std::string::npos)
        {
            addr_ = header_value.substr(0, pos);
            std::string port = header_value.substr(pos + 1);
            if (!port.empty())
                port_ = port;
        }
    }
    if (std::strncmp(buffer_.c_str() + bufferIndex, CRLF, 2) == 0)
    {
        bufferIndex += 2;
        if (Utils::needBody(method_))
            state_ = BODY;
        else
        {
            buffer_.clear();
            state_ = COMPLETE;
        }
    }
    else if (buffer_.size() == bufferIndex)
        state_ = Utils::needBody(method_) ? BODY : COMPLETE;
    else
        state_ = HEADER_NAME;
    return true;
}

HTTPRequest *HTTPRequestParser::makeRequest()
{
    if (state_ != COMPLETE)
        return NULL;
    try
    {
        HTTPRequest *request = new HTTPRequest(*this);
        return request;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return NULL;
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
    checkHeaders();
    for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    {
        const std::pair<std::string, std::string> &header = *it;
        if (Utils::isEqual(header.first, TRANSFER_ENCODING))
        {
            if (Utils::isEqual(header.second, CHUNKED))
            {
                // chunked 인코딩이 적용된 경우
                while (bufferIndex < buffer_.size())
                {
                    size_t pos = buffer_.find(CRLF, bufferIndex); // 청크의 크기를 나타내는 줄바꿈 위치 찾기
                    if (pos == std::string::npos)
                        throw ParseException();
                    std::string chunkSize = buffer_.substr(bufferIndex, pos - bufferIndex); // 청크의 크기를 나타내는 문자열
                    bufferIndex = pos + 2;

                    if (chunkSize.empty())
                        break; // 마지막 청크를 나타내는 빈 문자열인 경우 종료
                    if (2 < buffer_.size() && std::strncmp(buffer_.c_str() + (buffer_.size() - 6), "\n0", 2) != 0)
                        throw ParseException();

                    // chunk_size_str을 숫자로 변환
                    std::istringstream iss(chunkSize);
                    size_t chunk_size;
                    if (!(iss >> std::hex >> chunk_size))
                        throw ParseException();
                    if (chunk_size == 0)
                    {
                        buffer_.clear();
                        state_ = COMPLETE;
                        bodySize_ = body_.size();
                        // 만들어주면 안됌.
                        headers_.insert(std::make_pair(CONTENT_LENGTH, Utils::ftToString(bodySize_)));
                        return true;
                    }
                    if (buffer_.size() < chunk_size + 2)
                        throw ParseException();

                    std::string chunk_data = buffer_.substr(bufferIndex, chunk_size); // 청크의 데이터 추출
                    bufferIndex += chunk_size + 2;
                    body_ += chunk_data; // body에 청크 데이터 추가

                    if (buffer_.size() == 0)
                        throw ParseException();
                }
            }
        }
        else if (Utils::isEqual(header.first, CONTENT_LENGTH))
        {
            int contentLength = Utils::ftStoi(header.second);

            body_ = buffer_.substr(bufferIndex, contentLength);
            bodySize_ = body_.size();
            if (bodySize_ != contentLength)
                throw ParseException();
            bufferIndex += bodySize_;
            // TODO Bad request로 빠져야 함
            // body_ = buffer_.substr(bufferIndex, buffer_.size());
            // bodySize_ = body_.size();
            // headers_.insert(std::make_pair(CONTENT_LENGTH, Utils::ftToString(bodySize_)));
            // buffer_.clear();
            // state_ = COMPLETE;
            // return true;
        }
    }
    state_ = COMPLETE;
    return true;
}

void HTTPRequestParser::reset()
{
    state_ = METHOD;
    headers_.clear();
    body_.clear();
    buffer_.clear();
    currentHeaderName_.clear();
    query_.clear();
    addr_.clear();
    port_ = "80";
    path_.clear();
    httpVersion_.clear();
    chunkedData = "";
    bufferIndex = 0;
    bodySize_ = 0;
}

void HTTPRequestParser::checkHeaders()
{
    if (headers_.size() == 0)
        throw ParseException();
    for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    {
        const std::pair<std::string, std::string> &header = *it;
        if (header.second.empty())
            throw ParseException();
        if (Utils::isEqual(header.first, TRANSFER_ENCODING))
        {
            if (Utils::isEqual(header.second, CHUNKED))
            {
                if (buffer_.size() <= bufferIndex)
                    throw ParseException();
            }
        }
        else if (Utils::isEqual(header.first, CONTENT_LENGTH))
        {
            if (Utils::ftStoi(header.second) < 1)
                throw ParseException();
        }
    }
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
    std::cout << "Request HTTP version: " << result.httpVersion << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = result.headers.begin(); it != result.headers.end(); ++it)
        std::cout << "Header: " << it->first << " = " << it->second << std::endl;

    std::cout << "Body: " << result.body << std::endl;
}

const char *HTTPRequestParser::ParseException::what() const throw()
{
    return "HTTP Request Parse Exception";
}

std::string HTTPRequestParser::getMethod() const
{
    return method_;
}

std::string HTTPRequestParser::getPath() const
{
    return path_;
}

std::string HTTPRequestParser::getQuery() const
{
    return query_;
}

std::string HTTPRequestParser::getHttpVersion() const
{
    return httpVersion_;
}

std::map<std::string, std::string> HTTPRequestParser::getHeaders() const
{
    return headers_;
}

std::string HTTPRequestParser::getBody() const
{
    return body_;
}
