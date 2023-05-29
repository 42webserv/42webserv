/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:15 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:30:06 by sunhwang         ###   ########.fr       */
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

bool openIndexFile()
{
	return true;
}
