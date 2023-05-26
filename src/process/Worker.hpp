/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/26 17:23:03 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include <iostream>
#include <sys/event.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "MimeTypesParser.hpp"
#include "common_error.hpp"
#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "CGI.hpp"

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
	UData *responseUData;

	void eventEVError(int k, struct kevent &event);
	bool eventFilterRead(int k, struct kevent &event);
	bool eventFilterWrite(int k, struct kevent &event);
	bool eventEOF(int k, struct kevent &event);
	bool eventFilterTimer(int k, struct kevent &event);
	void requestHandler(const HTTPRequest &request, int client_fd);
	void getResponse(ResponseData *response);
	void errorResponse(int client_fd);
	std::string generateHeader(const std::string &content, const std::string &contentType);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(ResponseData *response);
	std::string getCGILocation(ResponseData *response);
	ResponseData *getResponseData(const HTTPRequest &request, const int &client_fd, ServerInfo &thisServer);
	void broad(ResponseData *response);
	void registerKeepAlive(const HTTPRequest *request, struct kevent &event, int client_fd);
	int findSocketIndex(struct kevent &event);
	bool checkHeaderIsKeepLive(const HTTPRequest *request);
	bool checkKeepLiveOptions(const HTTPRequest *request, struct kevent &event);
	void setTimer(int fd, int timeout);
	void deleteTimer(int fd);

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
