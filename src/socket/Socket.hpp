/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/27 22:52:17 by chanwjeo         ###   ########.fr       */
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

/*
 * event에 같이 들고 다닐 user에 대한 데이터이다.
 *
 * fd: client의 fd
 * keepLive: keep-alive인지 아닌지
 * isClient: client인지 아닌지
 */
struct UData
{
    UData(int fd,
          bool keepLive,
          bool isClient)
    {
        this->fd = fd;
        this->keepLive = keepLive;
        this->isClient = isClient;
        this->max = -1;
        this->timeout = -1;
        this->writeEventExist = false;
    };
    int fd;
    int max;
    int timeout;
    bool keepLive;
    bool isClient;
    bool writeEventExist;
};

class Socket
{
private:
    struct sockaddr_in _serverAddr;
    const int kq;

public:
    int _port;
    const int _serverFd;
    std::vector<int> _clientFds;
    Socket(std::vector<struct kevent> &eventList, const int port, const int kq);
    ~Socket();
    int handleEvent(std::vector<struct kevent> &eventList);
    void disconnectClient(int client_fd, std::map<int, std::string> &clients, struct kevent &event);
    static int enableKeepAlive(int socketFd);
    bool findClientFd(int client_fd);
};

#endif
