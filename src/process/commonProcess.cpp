/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:15 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/06 13:48:33 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include "commonProcess.hpp"

/**
 * @brief Send response to client
 * @param socket
 * @param buffer
 */
void ftSend(const int &socket, const std::string &buffer)
{
	if (buffer.empty())
		return;
	send(socket, buffer.c_str(), buffer.length(), 0);
}

/**
 * @brief Send response to client
 * @param response
 * @param contents
 */
void ftSend(ResponseData *response, const std::string &contents)
{
	ftSend(response->clientFd, contents);
}

void ftSend(ResponseData &response, const std::string &contents)
{
	ftSend(response.clientFd, contents);
}

bool isDirectory(const std::string &path)
{
	struct stat st;

	stat(path.c_str(), &st);
	if (S_ISDIR(st.st_mode))
		return true;
	return false;
}

bool isFile(const std::string &path)
{
	struct stat st;

	stat(path.c_str(), &st);
	if (S_ISREG(st.st_mode))
		return true;
	return false;
}

bool isMethod(const std::string &method)
{
	const std::string methods[] = {GET, HEAD, POST, PUT, PATCH, DELETE, CONNECT, TRACE, OPTIONS};

	for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++)
		if (method == methods[i])
			return true;
	return false;
}

bool writeFile(const std::string &path, const std::string &contents)
{
	std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);

	if (!file.is_open())
		return false;
	file << contents;
	file.close();
	return true;
}

std::string readFile(const std::string &path)
{
	std::ifstream file(path.c_str());

	if (!file.is_open())
		return "";
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return contents;
}

/**
 * HTTP 요청 메세지에서 Content-Type 헤더의 값을 반환
 *
 * @param request 파싱된 HTTP 요청
 * @return 문자열의 Content-Type 값 혹은 기본값
 */
const std::string getContentType(const HTTPRequest &request)
{
	const std::map<std::string, std::string> &headers = request.headers;

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
	{
		if (isEqual(it->first, "Content-Type"))
			return it->second;
	}
	return "text/plain";
}

std::string lower(const std::string &s)
{
	std::string lowerS = s;

	for (size_t i = 0; i < lowerS.size(); i++)
	{
		lowerS[i] = std::tolower(lowerS[i]);
	}
	return lowerS;
}

bool isEqual(const std::string &s1, const std::string &s2)
{
	std::string lowerS1 = lower(s1);
	std::string lowerS2 = lower(s2);

	return lowerS1 == lowerS2;
}

std::vector<Directive>::const_iterator findDirective(const std::vector<Directive> &directives, const std::string &name)
{
	for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
	{
		if (isEqual(it->name, name))
			return it;
	}
	return directives.end();
}

std::vector<Directive>::const_iterator findDirectiveNameValue(const std::vector<Directive> &directives, const std::string &name, const std::string &value)
{
	for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
	{
		if (isEqual(it->name, name) && isEqual(it->value, value))
			return it;
	}
	return directives.end();
}

int ftStoi(const std::string &str)
{
	std::stringstream ss(str);
	int requestBodySize;
	ss >> requestBodySize;

	return requestBodySize;
}
