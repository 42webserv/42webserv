/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:32:06 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/28 22:33:05 by sunhwang         ###   ########.fr       */
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
#include "CGI.hpp"

struct CGIData
{
    int port;
    std::string path;
    std::string body;
    std::string query;
    std::string addr;
    std::string name;
};

struct ResponseData
{
    int clientFd;
    std::string root;
    std::string index;
    std::string resourcePath;
    std::string contentType;
    std::string method;
    std::vector<std::string> limitExcept;
    std::string returnState;
    std::string redirect;
    Directive *location;
    std::string locationName;
    std::string path; // path중 locationName 부분을 지운 나머지 경로
    // CGIData *cgi;
    bool autoindex;
    std::string body;
    size_t contentLength;
};

class Response
{
private:
    int getSuitableServer(int port, Server &server);
    std::string getRootDirectory(const HTTPRequest &request, const ServerInfo &info);
    void setUpRoot(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpIndex(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpAutoindex(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpLimitExcept(std::vector<Directive> &locationBlock, ResponseData *response);
    void setUpReturnState(std::vector<Directive> &locationBlock, ResponseData *response);
    Directive *findLocation(const HTTPRequest &request, std::vector<Directive> &locations);

public:
    /*
     * A default constructor
     */
    Response();

    /*
     * A destructor
     */
    ~Response();

    /*
     * Add it if you feel necessary additional member functions.
     */
    ResponseData *getResponseData(const HTTPRequest &request, const int &client_fd, Config &config, Server &serverManger);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Response_HPP
