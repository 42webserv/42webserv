/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/11 19:45:27 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/12 15:00:49 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <vector>

class HTTPRequestParser;

struct HTTPRequest
{
	std::string method;
	int port;
	std::string path;
	std::string httpVersion;
	std::map<std::string, std::string> headers;
	std::string body;
	std::string query;
	std::string addr;
	std::string name;
	bool chunked;
	unsigned long long bodySize;
	unsigned int statusCode;
	bool keepAlive;
	std::map<std::string, std::string> cookies;
	//  SERVER_NAME 요청을 수신한 서버의 호스트 이름. -> conf에서 가져올것
	HTTPRequest &operator=(const HTTPRequest &ref);
	HTTPRequest(const HTTPRequestParser &parser);
};

#endif
