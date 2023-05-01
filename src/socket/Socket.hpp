/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 21:42:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 13:10:39 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/event.h>

class Socket
{
private:
    struct sockaddr_in server_addr;

public:
    const int server_fd;
    Socket(std::vector<struct kevent> &event_list);
    ~Socket();
    int handle_event(std::vector<struct kevent> &event_list) const;
    void disconnect_client(int client_fd, std::map<int, std::string> &clients) const;
};

#endif
