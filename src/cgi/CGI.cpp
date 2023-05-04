/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 15:06:27 by sanghan           #+#    #+#             */
/*   Updated: 2023/05/04 15:08:39 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(void)
{
}

CGI::~CGI(void)
{
}

CGI::CGI(const CGI &obj)
{
}

CGI CGI::&operator=(const CGI &obj)
{
}

// ~AUTH_TYPE
// 	공백
// -CONTENT_LENGTH
// 	content_Length
// -CONTENT_TYPE
// 	content type
// -GATEWAY_INTERFACE
// 	CGI/1.1()
// ~PATH_INFO
// 	request.get Uri()
// ~PATH_TRANSLATED
// 	request.get Uri()
// -QUERY_STRING
// 	getQueryFullPath
// -REMOTE_ADDR
// 	getClientIP
// ~REMOTE_IDENT
// 	공백
// ~REMOTE_USER
// 	공백
// -REQUEST_METHOD
// 	req.method
// ~REQUEST_URI
// 	getUri()
// -SCRIPT_NAME
// 	webserv/1.1()
// -SERVER_NAME
// 	server.\_serverName
// -SERVER_PORT
// 	server.\_serverPort
// -SERVER_PROTOCOL
// 	HTTP/1.1()
// -SERVER_SOFTWARE
// 	webserv/1.1()
