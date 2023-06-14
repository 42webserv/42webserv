/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/14 11:21:06 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <map>
#include <netinet/tcp.h>
#include <stdexcept>
#include <string>
#include <sys/event.h>
#include <vector>
#include "HTTPRequestParser.hpp"

#define BUFFER_SIZE 1024

class Socket
{
private:
    struct sockaddr_in _serverAddr;

public:
    const int _serverFd;
    std::vector<int> _clientFds;
    Socket(std::vector<struct kevent> &events, const int &port);
    ~Socket();
    Socket &operator=(const Socket &ref);
    Socket(const Socket &ref);
    void connectClient(std::vector<struct kevent> &events);
    bool receiveRequest(struct kevent &event);
    void disconnectClient(struct kevent &event);
    static int enableKeepAlive(int socketFd);
    void closeClients() const;
};

#endif
