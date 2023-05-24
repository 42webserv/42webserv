/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/24 15:59:28 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "Socket.hpp"
#include "common_error.hpp"
#include "Server.hpp"

Socket::Socket(std::vector<struct kevent> &event_list, const int port) : server_fd(socket(AF_INET, SOCK_STREAM, 0))
{
    struct kevent event;
    this->_port = port;

    // Create an AF_INET stream socket to receive incoming connections on
    if (server_fd < 0)
        error_exit("socket()");

    int on = 1;
    // Allow socket descriptor to be reuseable
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        close(server_fd);
        error_exit("setsockopt()");
    }

    // Bind the socket
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // TODO port는 server.port에서 받아야 함.
    server_addr.sin_port = htons(this->_port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(server_fd);
        error_exit("bind()");
    }

    // Set the listen back log
    if (listen(server_fd, 3) < 0)
    {
        close(server_fd);
        error_exit("listen()");
    }

    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    event_list.push_back(event);
    clientFds.push_back(server_fd);
    std::cout << "Server listening on port " << this->_port << std::endl;
}

Socket::~Socket()
{
    close(server_fd);
}

int Socket::handleEvent(std::vector<struct kevent> &event_list)
{
    socklen_t addrlen = sizeof(server_addr);
    struct sockaddr_in client_addr;
    struct kevent new_event;

    // Accept incoming connection
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0)
        error_exit("accept()");

    std::cout << "Accept new client:" << client_fd << std::endl;
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    // UData *uData = new UData(client_fd, false, true);
    // EV_SET(&new_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, uData);
    EV_SET(&new_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    event_list.push_back(new_event);
    clientFds.push_back(client_fd);
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
    clientFds.erase(std::remove(clientFds.begin(), clientFds.end(), client_fd), clientFds.end());
}

int Socket::enableKeepAlive(int socketFd)
{
    int keepAlive = 1;
    // int keepAliveTime = 15;
    int keepAliveInterval = 5;

    // SO_KEEPALIVE 옵션 활성화
    if (setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) < 0)
    {
        perror("setsockopt SO_KEEPALIVE");
        return -1;
    }
    std::cout << socketFd << " : keepalive on" << std::endl;
    // // TCP_KEEPIDLE 옵션 설정 (유휴 시간)
    // if (setsockopt(socketFd, IPPROTO_TCP, TCP_KEEPIDLE, &keepAliveTime, sizeof(keepAliveTime)) < 0)
    // {
    //     perror("setsockopt TCP_KEEPIDLE");
    //     return -1;
    // }

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

    for (it = clientFds.begin(); it != clientFds.end(); ++it)
    {
        if (*it == client_fd)
            return true;
    }
    return false;
}
