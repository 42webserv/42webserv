/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:10 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 16:43:58 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <string>
#include "Config.hpp"

struct ServerInfo
{
    int port;
    std::vector<Directive> location;
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
    int findListen(std::vector<Directive> &server);
    void setPortAndLocation(std::vector<Directive> &server);
    void printServer();
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Server_HPP
