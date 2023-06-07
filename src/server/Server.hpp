/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/07 15:05:50 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "commonError.hpp"
#include "Config.hpp"
#include "Socket.hpp"

struct ServerInfo
{
    std::vector<int> ports;
    std::vector<Socket *> sockets;
    size_t clientMaxBodySize;
    std::string serverName;
    std::string root;
    std::string index;
    std::vector<std::string> limitExcepts;
    std::vector<Directive> locations;
    std::map<int, std::string> errorPage;
    ServerInfo &operator=(const ServerInfo &ref);
    void closeSockets() const;
};

class Server
{
private:
    std::vector<int> validPorts;

    void setUpServer(std::vector<Directive> &servrBlocks, std::vector<struct kevent> &events);
    void setUpIndex(ServerInfo &tmpServ, std::vector<Directive> &servrBlocks);
    void setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks);
    void setUpLocation(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks);
    void setUpListen(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks);
    void setUpLimitExcept(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks);
    std::string findServerName(std::vector<Directive> &serverBlocks);
    size_t findClientMaxBodySize(std::vector<Directive> &serverBlocks);
    std::string findRoot(std::vector<Directive> &serverBlocks);

public:
    /* data */
    std::vector<ServerInfo> servers;

    /*
     * A default constructor
     */
    Server();

    /*
     * A copy constructor
     */
    Server(const Server &ref);

    /*
     * A assignment operator overload
     */
    Server &operator=(const Server &ref);

    /*
     * A destructor
     */
    ~Server();

    void printServer();
    void setServer(Config &config, std::vector<struct kevent> &events);
    ServerInfo &findServer(const int &fd);
    Socket *findSocket(const int &fd);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Server_HPP
