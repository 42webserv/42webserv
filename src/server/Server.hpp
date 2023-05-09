/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 19:18:42 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <string>
#include <sstream>
#include "Config.hpp"

struct ServerInfo
{
    int port;
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

    Server();
    /*
     * Add it if you feel necessary additional member functions.
     */
    void setUpServer(std::vector<Directive> &serverBlock);
    void setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlock);
    int findListen(std::vector<Directive> &serverBlock);
    std::string findServerName(std::vector<Directive> &serverBlock);
    size_t findClientMaxBodySize(std::vector<Directive> &serverBlock);
    std::string findRoot(std::vector<Directive> &serverBlock);

public:
    /*
     * A default constructor
     */
    Server(Config &config);

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
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Server_HPP
