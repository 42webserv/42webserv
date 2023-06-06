/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/06 20:41:34 by chanwjeo         ###   ########.fr       */
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
    // Socket *socket;
    std::vector<Socket *> sockets;
    size_t clientMaxBodySize;
    std::string serverName;
    std::string root;
    std::string index;
    std::vector<std::string> limitExcepts;
    std::vector<Directive> locations;
    std::map<int, std::string> errorPage;
    ServerInfo &operator=(const ServerInfo &ref);
};

class Server
{
private:
    std::vector<int> validPorts;

    /*
     * Add it if you feel necessary additional member functions.
     */
    void setUpServer(std::vector<Directive> &servrBlocks);
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
    void setServer(Config &config);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Server_HPP
