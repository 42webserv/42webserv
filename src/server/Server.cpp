/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:08 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 17:46:47 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
 * A default constructor
 */
Server::Server(Config &config)
{
    std::vector<Directive> server;
    config.getAllDirectives(server, config.getDirectives(), "server");
    setUpServer(server);
    printServer();
}

/*
 * A copy constructor
 */
Server::Server(const Server &ref)
{
    *this = ref;
}

/*
 * A assignment operator overload
 */
Server &Server::operator=(const Server &ref)
{
    if (this != &ref)
    {
        /* insert */
    }
    return *this;
}

/*
 * A destructor
 */
Server::~Server()
{
}

int Server::findListen(std::vector<Directive> &server)
{
    for (size_t j = 0; j < server.size(); j++)
    {
        if (server[j].name == "listen")
            return strtod(server[j].value.c_str(), NULL);
    }
    return 80;
}

std::string Server::findServerName(std::vector<Directive> &server)
{
    for (size_t j = 0; j < server.size(); j++)
    {
        if (server[j].name == "server_name")
            return server[j].value;
    }
    return "nobody";
}

void Server::setUpErrorPage(std::vector<Directive> &error, ServerInfo &tmpServ)
{
    for (size_t i = 0; i < error.size(); i++)
    {
        tmpServ.errorPage.push_back(error[i]);
    }
}

void Server::setUpServer(std::vector<Directive> &server)
{
    for (size_t i = 0; i < server.size(); i++)
    {
        ServerInfo tmpServ;
        tmpServ.port = findListen(server[i].block);
        tmpServ.serverName = findServerName(server[i].block);
        for (size_t j = 0; j < server[i].block.size(); j++)
        {
            if (server[i].block[j].name == "location")
                tmpServ.location.push_back(server[i].block[j]);
            else if (server[i].block[j].name == "error_page")
                setUpErrorPage(server[i].block[j].block, tmpServ);
        }
        this->server.push_back(tmpServ);
    }
}

void Server::printServer()
{
    std::cout << "============Server===========" << std::endl;
    for (size_t i = 0; i < this->server.size(); i++)
    {
        std::cout << "Server[" << i + 1 << "]" << std::endl;
        std::cout << "PORT: " << this->server[i].port << std::endl;
        std::cout << "Server_name: " << this->server[i].serverName << std::endl;
        for (size_t j = 0; j < this->server[i].location.size(); j++)
        {
            std::cout << "location : " << this->server[i].location[j].value << std::endl;
        }
        std::cout << "-------------------------------\n";
        for (size_t j = 0; j < this->server[i].errorPage.size(); j++)
            std::cout << "errorPage : " << this->server[i].errorPage[j].name << ", " << this->server[i].errorPage[j].value << std::endl;
        std::cout << "===============================\n\n";
    }
}
