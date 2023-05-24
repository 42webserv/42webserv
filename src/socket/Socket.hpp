/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/24 15:07:46 by seokchoi         ###   ########.fr       */
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
#include <netinet/tcp.h>

class Socket
{
private:
    struct sockaddr_in server_addr;

public:
    int _port;
    const int server_fd;
    std::vector<int> clientFds;
    Socket(std::vector<struct kevent> &event_list, const int port);
    ~Socket();
    int handleEvent(std::vector<struct kevent> &event_list);
    void disconnectClient(int client_fd, std::map<int, std::string> &clients);
    static int enableKeepAlive(int socketFd);
    bool findClientFd(int client_fd);
};

#endif
