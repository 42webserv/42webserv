/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/03 10:52:02 by sunhwang         ###   ########.fr       */
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
    UData(int fd, bool keepLive, bool isClient)
    {
        this->fd = fd;
        this->keepLive = keepLive;
        this->isClient = isClient;
        this->max = -1;
        this->timeout = -1;
        this->writeEventExist = false;
        this->sessionID = "";
        this->alreadySessionSend = false;
        this->sesssionValid = false;
        this->expireTime = "";
        this->wantToDeleteSessionInCookie = false;
        this->request = "";
    };
    int fd;
    int max;
    int timeout;
    bool keepLive;
    bool isClient;
    bool writeEventExist;
    std::string sessionID;
    bool alreadySessionSend;
    bool sesssionValid;
    std::string expireTime;
    bool wantToDeleteSessionInCookie;
    std::string request; // recv로 받아야 할 문자열
};

class Socket
{
private:
    struct sockaddr_in _serverAddr;
    const int kq;

public:
    const int _serverFd;
    std::vector<int> _clientFds;
    Socket(std::vector<struct kevent> &events, const int &port, const int &kq);
    ~Socket();
    Socket &operator=(const Socket &ref);
    Socket(const Socket &ref);
    int connectClient(std::vector<struct kevent> &events);
    void disconnectClient(struct kevent &event);
    static int enableKeepAlive(int socketFd);
    void closeClients() const;
};

#endif
