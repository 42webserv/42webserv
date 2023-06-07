/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 15:05:57 by chanwjeo         ###   ########.fr       */
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

/*
 * event에 같이 들고 다닐 user에 대한 데이터이다.
 *
 * fd: client의 fd
 * keepLive: keep-alive인지 아닌지
 * isClient: client인지 아닌지
 */
struct UData
{
    UData(int fd, bool keepLive, bool isClient)
    {
        this->fd = fd;
        this->keepLive = keepLive;
        this->isClient = isClient;
        this->max = -1;
        this->timeout = -1;
        this->sessionID = "";
        this->alreadySessionSend = false;
        this->sesssionValid = false;
        this->expireTime = "";
        this->wantToDeleteSessionInCookie = false;
        this->request = "";
        this->result = NULL;
    };
    int fd;
    int max;
    int timeout;
    bool keepLive;
    bool isClient;
    std::string sessionID;
    bool alreadySessionSend;
    bool sesssionValid;
    std::string expireTime;
    bool wantToDeleteSessionInCookie;
    std::string request; // recv로 받아야 할 문자열
    HTTPRequest *result;
};

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
    void receiveRequest(struct kevent &event);
    void disconnectClient(struct kevent &event);
    static int enableKeepAlive(int socketFd);
    void closeClients() const;
};

#endif
