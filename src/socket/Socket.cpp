/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/29 16:24:31 by seokchoi         ###   ########.fr       */
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

Socket::Socket(std::vector<struct kevent> &eventList, const int port, const int kq)
    : kq(kq), _serverFd(socket(AF_INET, SOCK_STREAM, 0))
{
    struct kevent event;
    this->_port = port;
    int opt;

    // Create an AF_INET stream socket to receive incoming connections on
    if (_serverFd < 0)
        errorExit("socket()");

    opt = 1;
    // Allow socket descriptor to be reuseable
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(_serverFd);
        errorExit("setsockopt()");
    }

    struct linger linger;
    linger.l_onoff = 1;
    linger.l_linger = 10;
    // CLOSE_WAIT 이후 10초가 지나면 소켓을 닫는다.
    if (setsockopt(_serverFd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) < 0)
    {
        close(_serverFd);
        errorExit("setsockopt()");
    }

    opt = 1;
    // Nagle 알고리즘 사용하지 않게 하기
    if (setsockopt(_serverFd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    // Bind the socket
    std::memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(this->_port);
    if (bind(_serverFd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    {
        close(_serverFd);
        errorExit("bind()");
    }

    // Set the listen back log
    if (listen(_serverFd, 3) < 0)
    {
        close(_serverFd);
        errorExit("listen()");
    }

    EV_SET(&event, _serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    eventList.push_back(event);
    _clientFds.push_back(_serverFd);
    std::cout << "Server listening on port " << this->_port << std::endl;
}

Socket::~Socket()
{
    for (std::vector<int>::iterator it = _clientFds.begin(); it != _clientFds.end(); it++)
        close(*it);
    close(_serverFd);
}

int Socket::handleEvent(std::vector<struct kevent> &eventList)
{
    socklen_t addrlen = sizeof(_serverAddr);
    struct sockaddr_in client_addr;
    struct kevent new_event;

    // Accept incoming connection
    int client_fd = accept(_serverFd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0)
        errorExit("accept()");

    std::cout << "Accept new client:" << client_fd << std::endl;
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    UData *uData = new UData(client_fd, false, true);
    EV_SET(&new_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, uData);

    struct linger lingerOption;
    lingerOption.l_onoff = 1;   // SO_LINGER 활성화
    lingerOption.l_linger = 10; // linger 시간을 10초로 설정

    // 소켓에 SO_LINGER 옵션 적용
    // SO_LINGER은 소켓이 close() 함수로 닫힐 때 송신 버퍼에 데이터가 남아있는 경우, 해당 데이터를 어떻게 처리할지를 제어하는 소켓 옵션입니다.
    if (setsockopt(client_fd, SOL_SOCKET, SO_LINGER, &lingerOption, sizeof(lingerOption)) < 0)
    {
        perror("setsockopt SO_LINGER");
    }

    eventList.push_back(new_event);
    _clientFds.push_back(client_fd);
    return client_fd;
}

void Socket::disconnectClient(int client_fd, std::map<int, std::string> &clients, struct kevent &event)
{
    if (event.udata)
        delete static_cast<UData *>(event.udata);
    event.udata = NULL;
    EV_SET(&event, client_fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    kevent(kq, &event, 1, NULL, 0, NULL);
    EV_SET(&event, client_fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    kevent(kq, &event, 1, NULL, 0, NULL);
    std::cout << "close client_fd: " << client_fd << std::endl;
    // shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    clients.erase(client_fd);
    _clientFds.erase(std::remove(_clientFds.begin(), _clientFds.end(), client_fd), _clientFds.end());
}

int Socket::enableKeepAlive(int socketFd)
{
    int keepAlive = 1;
    int keepAliveInterval = 30;

    // SO_KEEPALIVE 옵션 활성화
    if (setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) < 0)
    {
        perror("setsockopt SO_KEEPALIVE");
        return -1;
    }
    // TCP_KEEPINTVL 옵션 설정 (유휴 상태에서 keep-alive 패킷 간의 간격)
    if (setsockopt(socketFd, IPPROTO_TCP, TCP_KEEPINTVL, &keepAliveInterval, sizeof(keepAliveInterval)) < 0)
    {
        perror("setsockopt TCP_KEEPINTVL");
        return -1;
    }
    return 0;
}

bool Socket::findClientFd(int client_fd)
{
    std::vector<int>::iterator it;

    for (it = _clientFds.begin(); it != _clientFds.end(); ++it)
    {
        if (*it == client_fd)
            return true;
    }
    return false;
}
