/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 14:02:56 by sanghan          ###   ########.fr       */
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

#define BUFFER_SIZE 1024
#define CHUNK_SIZE 500

struct ResponseData;
class Master;

class Worker
{
private:
	const int &kq;
	const Signal signal;
	const Config &config;
	std::vector<struct kevent> &events;
	Server &server;
	HTTPRequestParser parser;

	void eventEOF(Socket &socket, struct kevent &event);
	void eventFilterRead(Socket &socket, struct kevent &event);
	void eventFilterTimer(Socket &socket, struct kevent &event);
	void eventFilterWrite(Socket &socket, struct kevent &event);
	void eventEVError(Socket &socket, struct kevent &event);
	void eventFilterSignal(struct kevent &event);
	void requestHandler(UData *udata, const int &clientFd);
	void getResponse(ResponseData *response);
	void postResponse(ResponseData *response, const HTTPRequest &request);
	void putResponse(ResponseData *response);
	void deleteResponse(ResponseData *response);
	std::string errorPageGenerator(int errorCode);
	std::string uploadPageGenerator(std::string executePath);
	void errorResponse(ResponseData *response, int errorCode);
	std::string generateHeader(const std::string &content, const std::string &contentType, int statusCode, bool chunked, UData *udata);
	std::string generateErrorHeader(int status_code, const std::string &message);
	bool isCGIRequest(const ResponseData &response);
	std::string getCGIPath(ResponseData &response);
	void broad(ResponseData *response);
	void registerKeepAlive(UData *udata, int clientFd);
	bool checkHeaderIsKeepLive(UData *udata);
	bool checkKeepLiveOptions(UData *udata);
	void setTimer(int fd, int timeout);
	void deleteTimer(int fd);
	std::string generateSessionID(int length);
	std::string getExpiryDate(int secondsToAdd);
	bool isCookieValid(const std::string &expireTime);
	void cookieCheck(UData *udata);
	void redirection(ResponseData *response);
	bool invalidResponse(ResponseData *response);
	// bool hasClientFd(const int &k);
	bool checkHttpRequestClientMaxBodySize(int k, const HTTPRequest &request, ResponseData *response);
	std::string extractSubstring(const std::string &A, const std::string &B, const std::string &C);
	void setResponse(ResponseData *response, const std::string &resourceContent);
	bool checkHttpRequestClientMaxBodySize(const HTTPRequest &request, ResponseData *response);
	void sendResponse(ResponseData *response, const HTTPRequest &request);

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
