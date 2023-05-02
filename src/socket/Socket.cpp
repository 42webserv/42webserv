/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 13:18:38 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "Socket.hpp"
#include "common_error.hpp"

#define PORT 8080

Socket::Socket(std::vector<struct kevent> &event_list) : server_fd(socket(AF_INET, SOCK_STREAM, 0))
{
    struct kevent event;

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
    server_addr.sin_port = htons(PORT);
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

    std::cout << "Server listening on port " << PORT << std::endl;
}

Socket::~Socket()
{
    close(server_fd);
}

int Socket::handle_event(std::vector<struct kevent> &event_list) const
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
    EV_SET(&new_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    event_list.push_back(new_event);
    return client_fd;
}

// TODO 이게 server랑 물려있는 client란 걸 어떻게 알까?
// 굳이 이 클래스의 맴버 변수를 쓰는 것도 아닌데 이 함수에 있을 필요가 있을지 모르겠네.
void Socket::disconnect_client(int client_fd, std::map<int, std::string> &clients) const
{
    close(client_fd);
    clients.erase(client_fd);
}
