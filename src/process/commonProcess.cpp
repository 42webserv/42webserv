/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:15 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/29 20:02:00 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	return !isDirectory(path);
}

bool isMethod(const std::string &method)
{
	const std::string methods[] = {GET, HEAD, POST, PUT, PATCH, DELETE, CONNECT, TRACE, OPTIONS};

	for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++)
		if (method == methods[i])
			return true;
	return false;
}
