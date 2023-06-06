/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:32:06 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/06 14:23:51 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Response_HPP
#define Response_HPP

#include "Config.hpp"
#include "HTTPRequestParser.hpp"
#include "MimeTypesParser.hpp"
#include "Server.hpp"
#include "Worker.hpp"

struct ResponseData
{
    int clientFd;
    std::string root;
    std::string index;
    std::string resourcePath;
    std::string contentType;
    std::string method;
    std::vector<std::string> limitExcept;
    std::vector<std::string> cgiPath;
    std::map<std::string, std::string> headers;
    std::string returnState;
    std::string redirect;
    Directive *location;
    std::string locationName;
    std::string path; // request의 path그대로 가져옴
    bool autoindex;
    std::string body;
    size_t contentLength;
    ServerInfo server;
    UData *udata;
    bool chunked;
    long long bodySize;
};

class Response
{
private:
    int getSuitableServer(int port, Server &serverManager);
    std::string getRootDirectory(const HTTPRequest &request, const ServerInfo &server);
    std::string delQuery(std::string path);
    void setUpRoot(ResponseData *response);
    void setUpIndex(ResponseData *response);
    void setUpAutoindex(ResponseData *response);
    void setUpLimitExcept(ResponseData *response);
    void setUpReturnState(ResponseData *response);
    void setUpCgiPath(ResponseData *respone);
    Directive *findLocation(const HTTPRequest &request, std::vector<Directive> &locations);
    std::string findMimeType(const std::string &path, Config &config);
    std::string getPath(const HTTPRequest &request, ResponseData *response);

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
    ResponseData *getResponseData(const HTTPRequest &request, const int &clientFd, Config &config, Server &serverManger);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // Response_HPP
