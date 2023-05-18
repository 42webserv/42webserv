/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/17 17:14:07 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include <iostream>
#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "server/Server.hpp"
#include "HTTPRequestParser.hpp"
#include "CGI.hpp"

struct Cgi
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
	std::ifstream resourceFile;
	std::string root;
	std::string index;
	std::string resourcePath;
	std::string contentType;
	std::vector<std::string> limit_except;
	std::string return_state;
	std::string redirect;
	Cgi *cgi;
	bool autoindex;
};

class Worker
{
private:
	const int kq;
	const Signal signal;
	std::vector<Socket *> sockets;
	std::vector<struct kevent> &event_list;
	Config config;
	Server server;

	void requestHandler(const HTTPRequest &request, int client_fd);
	void getResponse(ResponseData *response);
	void errorResponse(int client_fd);
	std::string generateHeader(const std::string &content, const std::string &contentType);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(const HTTPRequest &request);
	std::string extractCGIPath(const HTTPRequest &request);

	int getSuitableServer(int port);
	std::string getRootDirectory(const HTTPRequest &request, const ServerInfo &thisServer);
	ResponseData *getResponseData(const HTTPRequest &request, const int &client_fd, ServerInfo &thisServer);
	void Worker::broad(ResponseData *response)

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
