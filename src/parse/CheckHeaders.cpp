/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckHeaders.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/11 15:09:08 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 16:43:54 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CheckHeaders.hpp"
#include "commonConfig.hpp"
#include "Config.hpp"
#include "Utils.hpp"

Header::Header(const std::pair<std::string, std::string> &ref)
{
	this->key = ref.first;
	this->value = ref.second;
	this->lowerCaseKey = Utils::lower(this->key);
}

CheckHeaders::CheckHeaders(const std::map<std::string, std::string> &headers, HTTPRequest &request) : headers(headers), request(request)
{
	if (headers.empty())
		throw std::invalid_argument("Headers are empty");

	checkHasChunkInHeaders();
	const std::string checkHeaders[] = {"Host", "Connection", "Content-Length", "Cookie"};
	void (CheckHeaders::*headerfns[])(const Header &header) = {&CheckHeaders::processHost, &CheckHeaders::processConnection, &CheckHeaders::processContentLength, &CheckHeaders::processCookie};

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
	{
		const Header &header = Header(*it);

		for (size_t i = 0; i < sizeof(checkHeaders) / sizeof(checkHeaders[0]); i++)
		{
			if (Utils::isEqual(header.key, checkHeaders[i]))
			{
				(this->*headerfns[i])(header);
				break;
			}
		}
	}
}

/**
 * @brief Chunk를 사용하는지 확인한다.Content-Length 보다 먼저 확인되어야 한다.
 */
void CheckHeaders::checkHasChunkInHeaders()
{
	const std::string header = "Transfer-Encoding";
	std::map<std::string, std::string>::const_iterator it = headers.find(header);

	if (it != headers.end())
	{
		const Header &header = Header(*it);
		processTransferEncoding(header);
	}
	else
		this->request.chunked = false;
}

CheckHeaders::~CheckHeaders() {}

void CheckHeaders::processHost(const Header &header)
{
	const std::string &value = header.value;
	std::string host;
	std::string port;

	size_t pos = value.find(COLON);
	if (pos == std::string::npos)
		host = value;
	else
	{
		host = value.substr(0, pos);
		port = value.substr(pos + 1);
	}
	if (host == "localhost")
		host = "127.0.0.1";
	this->request.addr = host;
	if (!port.empty())
	{
		this->request.port = 80;
		if (port.find_first_not_of("0123456789") != std::string::npos)
			throw std::invalid_argument("Port is not digit");
		if (5 < port.size())
			throw std::invalid_argument("Port is too long");
		int portNum = Utils::ftStoi(port);
		if (portNum < 0 || 65535 < portNum)
			throw std::invalid_argument("Port is out of range");
		this->request.port = portNum;
	}
}

void CheckHeaders::processConnection(const Header &header)
{
	const std::string &value = header.value;

	if (value == "keep-alive")
		this->request.keepAlive = true;
	else if (value == "close")
		this->request.keepAlive = false;
	else
		throw std::invalid_argument("Connection is invalid");
}

// HTTP 요청에서 Content-Length는 선택사항입니다. GET 또는 DELETE의 경우 길이가 0이어야 합니다.
// POST의 경우, Content-Length가 지정되고 메시지 행의 길이와 일치하지 않으면 메시지는 잘리거나 지정된 길이까지 널로 채워집니다.

// Content-Length는 컨텐츠가 없는 경우(값이 0인 경우)에도 항상 HTTP 응답에서 리턴됩니다. -> Response에 해당하는 부분
void CheckHeaders::processContentLength(const Header &header)
{
	if (!this->request.chunked)
		return;
	const std::string &value = header.value;
	if (value.find_first_not_of("0123456789") != std::string::npos)
		throw std::invalid_argument("Content-Length is not digit");
	const size_t contentLength = Utils::ftStoi(value);
	if (contentLength < 0)
		throw std::invalid_argument("Content-Length is out of range");
	if (Utils::needBody(this->request.method))
	{
		const size_t bodySize = this->request.body.size();
		if (contentLength < bodySize)
			throw std::invalid_argument("Content-Length is too short");
		else if (bodySize < contentLength)
			throw std::invalid_argument("Content-Length is too long");
		this->request.bodySize = bodySize;
	}
}

void CheckHeaders::processTransferEncoding(const Header &header)
{
	const std::string &value = header.value;
	const std::string values[] = {"chunked", "compress", "deflate", "gzip", "identity"};

	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++)
	{
		if (value == values[i])
		{
			if (value == "chunked")
			{
				this->request.chunked = true;
				this->request.bodySize = this->request.body.size();
			}
			return;
		}
	}
	throw std::invalid_argument("Transfer-Encoding is invalid");
}

void CheckHeaders::processCookie(const Header &header)
{
	const std::string &value = header.value;
	const std::vector<std::string> values = Config::split(value, ' ');
	size_t pos;

	for (size_t i = 0; i < values.size(); i++)
	{
		std::vector<std::string> cookie = Config::split(values[i], '=');
		if (cookie.size() != 2)
			throw std::invalid_argument("Cookie is invalid");
		pos = cookie[1].find(';');
		if (pos != std::string::npos)
			throw std::invalid_argument("Cookie is invalid");
		cookie[1].erase(cookie[1].length() - 1);
		if (cookie[0].empty() || cookie[1].empty())
			throw std::invalid_argument("Cookie is invalid");
		this->request.cookies.insert(std::pair<std::string, std::string>(cookie[0], cookie[1]));
	}
}
