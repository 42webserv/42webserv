/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:32:06 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/18 20:37:06 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Response_HPP
#define Response_HPP

#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Worker.hpp"
#include "HTTPRequestParser.hpp"

struct ResponseData
{
    int clientFd;
    std::ifstream resourceFile;
    std::string root;
    std::string index;
    std::string resourcePath;
    std::string contentType;
    std::vector<std::string> limitExcept;
    std::string returnState;
    std::string redirect;
};

class Response
{
private:
    /* data */
    ServerInfo thisServer;

    int matchLocation(const HTTPRequest &request, ServerInfo &thisServer);
    int getSuitableServer(int port, Server &server);
    std::string getRootDirectory(const HTTPRequest &request, const ServerInfo &thisServer);
    void setUpRoot(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpIndex(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpLimitExcept(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpReturnState(std::vector<Directive> &locationBlock, ResponseData *response);

public:
    /*
     * A default constructor
     */
    Response(int port, Server &server);

    /*
     * A destructor
     */
    ~Response();

    /*
     * Add it if you feel necessary additional member functions.
     */
    ResponseData *getResponseData(const HTTPRequest &request, const int &client_fd);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Response_HPP
