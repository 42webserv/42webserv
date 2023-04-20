/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:19 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/20 16:55:51 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum HttpMethod
{
    GET,
    POST
};

struct HttpRequest
{
    HttpMethod method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
};

class HttpRequestParser
{
public:
    HttpRequestParser() : state_(METHOD) {}

    HttpRequest *parse(const std::string &data)
    {
        buffer_ += data;

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
                if (!parseHttpVersion())
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
            HttpRequest *request = new HttpRequest;
            request->method = method_;
            request->path = path_;
            request->http_version = http_version_;
            request->headers = headers_;
            request->body = body_;
            reset();
            return request;
        }

        std::cout << "here8" << std::endl;
        return NULL;
    }

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
    HttpMethod method_;
    std::string path_;
    std::string http_version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string buffer_;
    std::string current_header_name_;

    bool parseMethod()
    {
        size_t pos = buffer_.find(' ');
        if (pos == std::string::npos)
            return false;
        std::string method_str = buffer_.substr(0, pos);
        if (method_str == "GET")
            method_ = GET;
        else if (method_str == "POST")
            method_ = POST;
        else
            return false;
        state_ = PATH;
        buffer_.erase(0, pos + 1);
        return true;
    }

    bool parsePath()
    {
        size_t pos = buffer_.find(' ');
        if (pos == std::string::npos)
            return false;
        path_ = buffer_.substr(0, pos);
        state_ = HTTP_VERSION;
        buffer_.erase(0, pos + 1);
        return true;
    }

    bool parseHttpVersion()
    {
        size_t pos = buffer_.find("\r\n");
        if (pos == std::string::npos)
            return false;
        http_version_ = buffer_.substr(0, pos);
        state_ = HEADER_NAME;
        buffer_.erase(0, pos + 2);
        return true;
    }

    bool parseHeaderName()
    {
        size_t pos = buffer_.find(':');
        if (pos == std::string::npos)
            return false;
        current_header_name_ = buffer_.substr(0, pos);
        buffer_.erase(0, pos + 1);
        state_ = HEADER_VALUE;
        return true;
    }

    bool parseHeaderValue()
    {
        size_t pos = buffer_.find("\r\n");
        if (pos == std::string::npos)
            return false;
        std::string header_value = buffer_.substr(0, pos);
        headers_.insert(std::make_pair(current_header_name_, header_value));
        buffer_.erase(0, pos + 2);
        if (buffer_.substr(0, 2) == "\r\n")
        {
            buffer_.erase(0, 2);
            state_ = BODY;
        }
        else
        {
            state_ = HEADER_NAME;
        }
        return true;
    }

    bool parseBody()
    {
        if (method_ == POST)
        {
            std::map<std::string, std::string>::iterator it =
                headers_.find("Content-Length");
            if (it != headers_.end())
            {
                int content_length = atoi(it->second.c_str());
                if (buffer_.size() < static_cast<size_t>(content_length))
                    return false;
                body_ = buffer_.substr(0, content_length);
                buffer_.erase(0, content_length);
            }
        }
        state_ = COMPLETE;
        return true;
    }

    void reset()
    {
        state_ = METHOD;
        headers_.clear();
        body_.clear();
        buffer_.clear();
        current_header_name_.clear();
    }
};

int main()
{
    // Example usage of HttpRequestParser
    HttpRequestParser parser;
    std::string http_request =
        "POST /example/path HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: ExampleBrowser/1.0\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 11\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello World";
    HttpRequest *result = parser.parse(http_request);

    if (result)
    {
        std::cout << "Request method: " << (result->method == GET ? "GET" : "POST") << std::endl;
        std::cout << "Request path: " << result->path << std::endl;
        std::cout << "Request HTTP version: " << result->http_version << std::endl;

        for (std::map<std::string, std::string>::const_iterator it = result->headers.begin(); it != result->headers.end(); ++it)
            std::cout << "Header: " << it->first << " = " << it->second << std::endl;
        delete result;
    }
    else
    {
        std::cout << result << std::endl;
        std::cout << "Failed to parse request" << std::endl;
    }

    return 0;
}
