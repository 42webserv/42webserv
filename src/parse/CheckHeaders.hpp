/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckHeaders.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/11 15:09:15 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/11 22:53:36 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECK_HEADERS_HPP
#define CHECK_HEADERS_HPP

#include <string>
#include <stdexcept>
#include "HTTPRequest.hpp"

struct Header
{
	std::string key;
	std::string value;
	std::string lowerCaseKey;
	Header(const std::pair<std::string, std::string> &ref);
};

class CheckHeaders
{
private:
	const std::map<std::string, std::string> &headers;
	HTTPRequest &request;

public:
	// HTTP_HEADERS_IN
	void processHost(const Header &header);
	void processConnection(const Header &header);
	void processContentLength(const Header &header);
	void processContentType(const Header &header);
	void processTransferEncoding(const Header &header);
	void processCookie(const Header &header);

public:
	CheckHeaders(const std::map<std::string, std::string> &headers, HTTPRequest &request);
	~CheckHeaders();
};

#endif
