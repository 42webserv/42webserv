/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckHeaders.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/11 15:09:08 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/11 23:11:13 by sunhwang         ###   ########.fr       */
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

	const std::string checkHeaders[] = {"Host", "Connection", "Content-Length", "Transfer-Encoding", "Cookie"};
	void (CheckHeaders::*headerfns[])(const Header &header) = {&CheckHeaders::processHost, &CheckHeaders::processConnection, &CheckHeaders::processContentLength, &CheckHeaders::processTransferEncoding, &CheckHeaders::processCookie};

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

void CheckHeaders::processContentLength(const Header &header)
{
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
		else if (contentLength > bodySize)
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
			return;
	}
	throw std::invalid_argument("Transfer-Encoding is invalid");

	if (value == "chunked")
	{
		if (this->request.body.empty())
			throw std::invalid_argument("Body is empty");
		this->request.bodySize = this->request.body.size();
	}
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
