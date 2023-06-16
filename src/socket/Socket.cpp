/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/15 13:14:47 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <iostream>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include "commonError.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "color.hpp"

Socket::Socket(std::vector<struct kevent> &events, const int &port) : _serverFd(socket(AF_INET, SOCK_STREAM, 0))
{
    struct kevent event;
    struct linger linger;
    int opt;

    // Create an AF_INET stream socket to receive incoming connections on
    if (_serverFd < 0)
        stderrExit("socket() error");

    if (fcntl(this->_serverFd, F_SETFL, O_NONBLOCK) < 0)
        stderrExit("fcntl non-block failed\n");

    opt = 1;
    // Allow socket descriptor to be reuseable
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        stderrExit("setsockopt() error");

    linger.l_onoff = 1;
    linger.l_linger = 0;
    // CLOSE_WAIT 이후 10초가 지나면 소켓을 닫는다.
    if (setsockopt(_serverFd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) < 0)
        stderrExit("setsockopt() error");

    opt = 1;
    // Nagle 알고리즘 사용하지 않게 하기
    if (setsockopt(_serverFd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
        stderrExit("setsockopt() error");

    memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(port);

    // Bind the socket
    if (bind(_serverFd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
        stderrExit("bind() error");

    // Set the listen back log
    if (listen(_serverFd, SOMAXCONN) < 0)
    {
        close(_serverFd);
        stderrExit("listen() error");
    }
    memset(&event, 0, sizeof(struct kevent));
    EV_SET(&event, _serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    events.push_back(event);
    std::cout << BBLK "🛠 port " BRED << port << BBLK " ready" << std::endl;
}

Socket::Socket(const Socket &ref) : _serverFd(ref._serverFd)
{
    if (this == &ref)
        return;
    *this = ref;
}

Socket &Socket::operator=(const Socket &ref)
{
    if (this != &ref)
    {
        this->_serverAddr = ref._serverAddr;
        this->_clientFds = ref._clientFds;
    }
    return *this;
}

Socket::~Socket()
{
    for (std::vector<int>::iterator it = _clientFds.begin(); it != _clientFds.end(); it++)
        close(*it);
    _clientFds.clear();
    close(_serverFd);
}

void Socket::connectClient(std::vector<struct kevent> &events)
{
    socklen_t addrlen = sizeof(_serverAddr);
    struct sockaddr_in clientAddr;
    struct kevent event;
    UData *udata;

    // Accept incoming connection
    int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &addrlen);
    if (clientFd < 0)
        throw(std::runtime_error("accept() error"));

    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
        throw(std::runtime_error("fcntl non-block failed\n"));
    udata = new UData(clientFd, false, true); // 처음 udata 생성
    EV_SET(&event, clientFd, EVFILT_READ, EV_ADD, 0, 0, udata);
    struct linger lingerOption;
    lingerOption.l_onoff = 1;  // SO_LINGER 활성화
    lingerOption.l_linger = 0; // linger 시간을 10초로 설정

    // 소켓에 SO_LINGER 옵션 적용
    // SO_LINGER은 소켓이 close() 함수로 닫힐 때 송신 버퍼에 데이터가 남아있는 경우, 해당 데이터를 어떻게 처리할지를 제어하는 소켓 옵션입니다.
    if (setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &lingerOption, sizeof(lingerOption)) < 0)
        throw(std::runtime_error("setsockopt SO_LINGER error"));

    events.push_back(event);
    _clientFds.push_back(clientFd);
    std::cout << "\r" BYEL "🔌 ACCEPT " << END << std::endl;
}

void Socket::receiveRequest(struct kevent &event)
{
    const int &fd = event.ident;
    UData *udata = static_cast<UData *>(event.udata);
    char buf[BUFFER_SIZE];
    ssize_t n;

    memset(buf, 0, BUFFER_SIZE);
    while (true)
    {
        n = recv(fd, buf, BUFFER_SIZE - 1, 0);
        if (n < 0)
            break;
        else
        {
            buf[n] = '\0';
            udata->request.append(buf);
            if (n < BUFFER_SIZE - 1)
                break;
        }
    }
}

void Socket::disconnectClient(struct kevent &event)
{
    const int &clientFd = event.ident;
    UData *udata = static_cast<UData *>(event.udata);

    udata->request.clear();
    if (udata->result)
        delete udata->result;
    if (udata)
        delete udata;
    event.udata = NULL;
    _clientFds.erase(std::remove(_clientFds.begin(), _clientFds.end(), clientFd), _clientFds.end());
    close(clientFd);
    // std::cout << BRED "\r🔌" << clientFd << " disconnect " END << std::endl;
}

int Socket::enableKeepAlive(int socketFd)
{
    int keepAlive = 1;
    int keepAliveInterval = 30;

    // SO_KEEPALIVE 옵션 활성화
    if (setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) < 0)
        throw(std::runtime_error("setsockopt SO_KEEPALIVE error"));
    // TCP_KEEPINTVL 옵션 설정 (유휴 상태에서 keep-alive 패킷 간의 간격)
    if (setsockopt(socketFd, IPPROTO_TCP, TCP_KEEPINTVL, &keepAliveInterval, sizeof(keepAliveInterval)) < 0)
        throw(std::runtime_error("setsockopt TCP_KEEPINTVL error"));
    return 0;
}

void Socket::closeClients() const
{
    for (std::vector<int>::const_iterator it = this->_clientFds.begin(); it != this->_clientFds.end(); it++)
    {
        const int &clientFd = *it;
        close(clientFd);
    }
}
