/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 18:50:03 by yje              ###   ########.fr       */
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
	void getResponse(const HTTPRequest &request, int client_fd);
	void errorResponse(int client_fd);
	std::string generateHeader(const std::string &content, const std::string &contentType);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(const HTTPRequest &request);
	std::string	extractCGIPath (const HTTPRequest &request);



public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
