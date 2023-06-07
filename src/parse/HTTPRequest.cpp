/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:21:43 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/06 19:51:03 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &ref)
{
	if (this != &ref)
	{
		this->method = ref.method;
		this->port = ref.port;
		this->path = ref.path;
		this->http_version = ref.http_version;
		this->headers = ref.headers;
		this->body = ref.body;
		this->query = ref.query;
		this->addr = ref.addr;
		this->name = ref.name;
		this->chunked = ref.chunked;
		this->bodySize = ref.bodySize;
	}
	return *this;
}
