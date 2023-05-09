/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:08 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 19:12:40 by chanwjeo         ###   ########.fr       */
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
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i].name == "listen")
            return strtod(server[i].value.c_str(), NULL);
    }
    return 80;
}

std::string Server::findServerName(std::vector<Directive> &server)
{
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i].name == "server_name")
            return server[i].value;
    }
    return "nobody";
}

size_t Server::findClientMaxBodySize(std::vector<Directive> &server)
{
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i].name == "client_max_body_size")
            return static_cast<size_t>(strtod(server[i].value.c_str(), NULL));
    }
    return -1;
}

void Server::setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &server)
{
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i].name == "error_page")
        {
            std::vector<std::string> tokens;
            std::istringstream iss(server[i].value);
            std::string token;

            while (iss >> token)
                tokens.push_back(token);
            std::string errorPage = tokens[tokens.size() - 1];
            for (size_t j = 0; j < tokens.size() - 1; j++)
            {
                int key = static_cast<int>(strtod(tokens[j].c_str(), NULL));
                tmpServ.errorPage[key] = errorPage;
            }
        }
    }
}

void Server::setUpServer(std::vector<Directive> &server)
{
    for (size_t i = 0; i < server.size(); i++)
    {
        ServerInfo tmpServ;
        tmpServ.port = findListen(server[i].block);
        tmpServ.serverName = findServerName(server[i].block);
        tmpServ.clientMaxBodySize = findClientMaxBodySize(server[i].block);
        setUpErrorPage(tmpServ, server[i].block);
        for (size_t j = 0; j < server[i].block.size(); j++)
        {
            if (server[i].block[j].name == "location")
                tmpServ.location.push_back(server[i].block[j]);
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
        std::cout << "Client_max_body_size: " << this->server[i].clientMaxBodySize << std::endl;
        for (size_t j = 0; j < this->server[i].location.size(); j++)
        {
            std::cout << "location : " << this->server[i].location[j].value << std::endl;
        }
        std::cout << "-------------------------------\n";
        for (std::map<int, std::string>::iterator iter = this->server[i].errorPage.begin(); iter != this->server[i].errorPage.end(); iter++)
            std::cout << "errorPage : " << iter->first << ", " << iter->second << std::endl;
        std::cout << "===============================\n\n";
    }
}
