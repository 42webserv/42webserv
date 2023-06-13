/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:21:43 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 16:32:39 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CheckHeaders.hpp"
#include "HTTPRequest.hpp"
#include "Utils.hpp"

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &ref)
{
	if (this != &ref)
	{
		this->method = ref.method;
		this->port = ref.port;
		this->path = ref.path;
		this->httpVersion = ref.httpVersion;
		this->headers = ref.headers;
		this->body = ref.body;
		this->query = ref.query;
		this->addr = ref.addr;
		this->name = ref.name;
		this->chunked = ref.chunked;
		this->bodySize = ref.bodySize;
		this->statusCode = ref.statusCode;
		this->keepAlive = ref.keepAlive;
	}
	return *this;
}

HTTPRequest::HTTPRequest(const HTTPRequestParser &parser)
{
	this->statusCode = 200;
	// Start Line
	this->method = parser.getMethod();
	this->path = parser.getPath();
	this->httpVersion = parser.getHttpVersion();
	this->query = parser.getQuery();

	// Headers
	this->keepAlive = false;
	std::map<std::string, std::string> headers = parser.getHeaders();
	CheckHeaders checkHeaders(headers, *this);
	this->headers = headers;

	// Body
	// TODO PUT과 POST에 Body가 있는 상태인데 header에 Content-Length와 Transfer가 없는 경우
	this->body = parser.getBody();
	this->bodySize = this->body.size();
}
