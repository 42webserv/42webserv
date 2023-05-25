/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:23:41 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "commonError.hpp"
#include "Socket.hpp"

Socket::Socket(std::vector<struct kevent> &eventList, const int port) : _serverFd(socket(AF_INET, SOCK_STREAM, 0))
{
    struct kevent event;
    this->_port = port;

    // Create an AF_INET stream socket to receive incoming connections on
    if (_serverFd < 0)
        errorExit("socket()");

    int on = 1;
    // Allow socket descriptor to be reuseable
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        close(_serverFd);
        errorExit("setsockopt()");
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
    EV_SET(&new_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    eventList.push_back(new_event);
    _clientFds.push_back(client_fd);
    return client_fd;
}

// TODO 이게 server랑 물려있는 client란 걸 어떻게 알까?
// 굳이 이 클래스의 맴버 변수를 쓰는 것도 아닌데 이 함수에 있을 필요가 있을지 모르겠네.
void Socket::disconnectClient(int client_fd, std::map<int, std::string> &clients)
{
    // TODO keep-alive면 안지운다. timeout도 해보기.
    // disconnect하기 전에 이벤트 삭제도 등록하기.
    close(client_fd);
    clients.erase(client_fd);
    _clientFds.erase(std::remove(_clientFds.begin(), _clientFds.end(), client_fd), _clientFds.end());
}
