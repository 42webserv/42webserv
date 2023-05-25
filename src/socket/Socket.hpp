/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 14:42:55 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/event.h>
#include <vector>

class Socket
{
private:
    struct sockaddr_in _serverAddr;

public:
    int _port;
    const int _serverFd;
    std::vector<int> _clientFds;
    Socket(std::vector<struct kevent> &eventList, const int port);
    ~Socket();
    int handleEvent(std::vector<struct kevent> &eventList);
    void disconnectClient(int clientFd, std::map<int, std::string> &clients);
};

#endif
