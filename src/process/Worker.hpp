/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/24 13:57:22 by chanwjeo         ###   ########.fr       */
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
