/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/03 15:35:30 by sunhwang         ###   ########.fr       */
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
#include "commonConfig.hpp"
#include "commonError.hpp"
#include "commonProcess.hpp"
#include "commonUtils.hpp"
#include "HTTPRequestParser.hpp"
#include "Server.hpp"
#include "Signal.hpp"
#include "Socket.hpp"

class Master;
struct ResponseData;

class Worker
{
private:
	const int &kq;
	const Signal signal;
	std::vector<struct kevent> &events;
	Config &config;
	Server &server;
	HTTPRequestParser parser;
	std::vector<Directive> listen;
	UData *udata;

	void eventEOF(Socket &socket, struct kevent &event);
	void eventFilterRead(Socket &socket, struct kevent &event);
	void eventFilterTimer(Socket &socket, struct kevent &event);
	void eventFilterWrite(Socket &socket, struct kevent &event);
	void eventEVError(Socket &socket, struct kevent &event);
	void eventFilterSignal(struct kevent &event);
	void requestHandler(const HTTPRequest &request, const int &clientFd);
	void getResponse(ResponseData *response);
	void postResponse(ResponseData *response);
	void putResponse(ResponseData *response);
	void deleteResponse(ResponseData *response);
	std::string errorPageGenerator(int errorCode);
	void errorResponse(ResponseData *response, int errorCode);
	std::string generateHeader(const std::string &content, const std::string &contentType, int statusCode);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(const ResponseData &response);
	std::string getCGILocation(ResponseData *response);
	std::string getCGIPath(ResponseData &response);
	void broad(ResponseData *response);
	void registerKeepAlive(const HTTPRequest *request, struct kevent &event, int clientFd);
	bool checkHeaderIsKeepLive(const HTTPRequest *request);
	bool checkKeepLiveOptions(const HTTPRequest *request);
	void setTimer(int fd, int timeout);
	void deleteTimer(int fd);
	std::string generateSessionID(int length);
	std::string getExpiryDate(int secondsToAdd);
	bool isCookieValid(const std::string &expireTime);
	void cookieCheck(HTTPRequest *result);
	void redirection(ResponseData *response);
	bool invalidResponse(ResponseData *response);
	bool checkHttpRequestClientMaxBodySize(const HTTPRequest &request, ResponseData *response);

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
