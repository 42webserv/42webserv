/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/27 22:48:25 by chanwjeo         ###   ########.fr       */
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
    std::vector<int> port;
    // Socket *socket;
    std::vector<Socket *> sockets;
    size_t clientMaxBodySize;
    std::string serverName;
    std::string root;
    std::string index;
    std::vector<std::string> limitExcept;
    std::vector<Directive> location;
    std::map<int, std::string> errorPage;
};

class Server
{
private:
    std::vector<int> validPort;

    /*
     * Add it if you feel necessary additional member functions.
     */
    void setUpServer(std::vector<Directive> &servrBlock);
    void setUpIndex(ServerInfo &tmpServ, std::vector<Directive> &servrBlock);
    void setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    void setUpLocation(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    void setUpListen(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    void setUpLimitExcept(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    std::string findServerName(std::vector<Directive> &serverBlock);
    size_t findClientMaxBodySize(std::vector<Directive> &serverBlock);
    std::string findRoot(std::vector<Directive> &serverBlock);

public:
    /* data */
    std::vector<ServerInfo> server;

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
