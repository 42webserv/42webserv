/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:23:25 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include <dirent.h>
#include <iostream>
#include <sys/event.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CGI.hpp"
#include "commonError.hpp"
#include "Config.hpp"
#include "HTTPRequestParser.hpp"
#include "Master.hpp"
#include "MimeTypesParser.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Signal.hpp"
#include "Socket.hpp"

#define BUFFER_SIZE 1024

struct CGIData;
struct ResponseData;
struct HTTPRequest;

class Worker
{
private:
	const int kq;
	const Signal signal;
	std::vector<Socket *> sockets;
	std::vector<struct kevent> &event_list;
	std::map<int, std::string> clients;
	std::vector<int>::iterator found;
	int fd;
	Config config;
	Server server;
	HTTPRequestParser parser;

	void eventEVError(int k);
	bool eventFilterRead(int k);
	bool eventFilterWrite(int k);
	void requestHandler(const HTTPRequest &request, int client_fd);
	void getResponse(ResponseData *response);
	void postResponse(ResponseData *response);
	void errorResponse(int client_fd);
	std::string generateHeader(const std::string &content, const std::string &contentType);
	// post 응답 201 > generateHeader 안에서 구현 또는 새로...?
	std::string tempPostHeader(const std::string &content, const std::string &contentType);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(ResponseData *response);
	std::string getCGILocation(ResponseData *response);
	ResponseData *getResponseData(const HTTPRequest &request, const int &client_fd, ServerInfo &thisServer);
	void broad(ResponseData *response);

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
