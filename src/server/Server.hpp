/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 18:56:00 by chanwjeo         ###   ########.fr       */
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
    std::string serverName;
    std::vector<Directive> location;
    std::map<int, std::string> errorPage;
};

class Server
{
private:
    /* data */
    std::vector<ServerInfo> server;

    Server();

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

    /*
     * Add it if you feel necessary additional member functions.
     */
    void setUpServer(std::vector<Directive> &server);
    void setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &server);
    int findListen(std::vector<Directive> &server);
    std::string findServerName(std::vector<Directive> &server);

    void printServer();
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Server_HPP
