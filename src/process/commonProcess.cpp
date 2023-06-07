/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:15 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 17:15:22 by chanwjeo         ###   ########.fr       */
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
void ftSend(const ResponseData *response, const std::string &contents)
{
	ftSend(response->clientFd, contents);
}

void ftSend(const ResponseData &response, const std::string &contents)
{
	ftSend(response.clientFd, contents);
}

/**
 * @brief Check if the path is a directory
 * @param path
 */
bool isDirectory(const std::string &path)
{
	struct stat st;

	stat(path.c_str(), &st);
	if (S_ISDIR(st.st_mode))
		return true;
	return false;
}

/**
 * @brief Check if the path is a file
 * @param path
 */
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
