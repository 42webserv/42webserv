/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 19:09:51 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/03 00:34:22 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "Server.hpp"

ServerInfo &ServerInfo::operator=(const ServerInfo &ref)
{
	if (this != &ref)
	{
		this->ports = ref.ports;
		this->sockets = ref.sockets;
		this->clientMaxBodySize = ref.clientMaxBodySize;
		this->serverName = ref.serverName;
		this->root = ref.root;
		this->index = ref.index;
		this->limitExcepts = ref.limitExcepts;
		this->locations = ref.locations;
		this->errorPage = ref.errorPage;
	}
	return (*this);
}

void ServerInfo::closeSockets() const
{
	for (std::vector<Socket *>::const_iterator it = this->sockets.begin(); it != this->sockets.end(); it++)
	{
		const Socket *socket = *it;
		socket->closeClients();
		close(socket->_serverFd);
		delete socket;
	}
}
