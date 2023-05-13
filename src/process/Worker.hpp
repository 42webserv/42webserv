/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/11 18:14:40 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "HTTPRequestParser.hpp"

struct ResponseData
{
	int clientFd;
	std::ifstream resourceFile;
	std::string root;
	std::string index;
	std::string resourcePath;
	std::string contentType;
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
	int getSuitableServer(int port);
	std::string getRootDirectory(const HTTPRequest &request, ServerInfo &thisServer);

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
