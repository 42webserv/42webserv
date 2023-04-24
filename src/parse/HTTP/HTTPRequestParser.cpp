#include "HTTPRequestParser.hpp"

HTTPRequestParser::HTTPRequestParser() : state_(METHOD) {}

HTTPRequest *HTTPRequestParser::parse(const std::string &data)
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
        HTTPRequest *request = new HTTPRequest;
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

bool HTTPRequestParser::parseMethod()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    std::string method_str = buffer_.substr(0, pos);
    if (method_str == "GET")
        method_ = GET;
    else if (method_str == "HEAD")
        method_ = POST;
    else if (method_str == "POST")
        method_ = POST;
    else if (method_str == "PUT")
        method_ = POST;
    else if (method_str == "PATCH")
        method_ = POST;
    else if (method_str == "DELETE")
        method_ = POST;
    else if (method_str == "CONNECT")
        method_ = POST;
    else if (method_str == "TRACE")
        method_ = POST;
    else if (method_str == "OPTIONS")
        method_ = POST;
    else
        return false;
    state_ = PATH;
    buffer_.erase(0, pos + 1);
    return true;
}

bool HTTPRequestParser::parsePath()
{
    size_t pos = buffer_.find(' ');
    if (pos == std::string::npos)
        return false;
    path_ = buffer_.substr(0, pos);
    state_ = HTTP_VERSION;
    buffer_.erase(0, pos + 1);
    return true;
}

bool HTTPRequestParser::parseHttpVersion()
{
    size_t pos = buffer_.find("\r\n");
    if (pos == std::string::npos)
        return false;
    http_version_ = buffer_.substr(0, pos);
    state_ = HEADER_NAME;
    buffer_.erase(0, pos + 2);
    return true;
}

bool HTTPRequestParser::parseHeaderName()
{
    size_t pos = buffer_.find(':');
    if (pos == std::string::npos)
        return false;
    current_header_name_ = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 1);
    state_ = HEADER_VALUE;
    return true;
}

bool HTTPRequestParser::parseHeaderValue()
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

bool HTTPRequestParser::parseBody()
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

void HTTPRequestParser::reset()
{
    state_ = METHOD;
    headers_.clear();
    body_.clear();
    buffer_.clear();
    current_header_name_.clear();
}
