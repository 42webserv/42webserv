/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/10 15:49:26 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Config.hpp"
#include "Socket.hpp"
struct ServerInfo
{
    std::vector<int> port;
    size_t clientMaxBodySize;
    std::string serverName;
    std::string root;
    std::vector<Directive> location;
    std::map<int, std::string> errorPage;
};

class Server
{
private:
    /* data */
    std::vector<ServerInfo> server;
    std::vector<*Socket> sockets;
    /*
     * Add it if you feel necessary additional member functions.
     */
    void setUpServer(std::vector<Directive> &servrBlock);
    void setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    void setUpLocation(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    void setUpListen(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    std::string findServerName(std::vector<Directive> &serverBlock);
    size_t findClientMaxBodySize(std::vector<Directive> &serverBlock);
    std::string findRoot(std::vector<Directive> &serverBlock);

public:
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
