/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/02 18:05:55 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
#include "commonProcess.hpp"
#include "Worker.hpp"

Worker::Worker(Master &master) : kq(master.kq), signal(master.getEvents()), event_list(master.getEvents()), config(master.getConfig()), server(master.getServer())
{
	// Create sockets
	for (size_t i = 0; i < server.servers.size(); i++)
	{
		for (size_t j = 0; j < server.servers[i].ports.size(); j++)
		{
			Socket *socket = new Socket(master.getEvents(), server.servers[i].ports[j], kq);
			sockets.push_back(socket);
		}
	}
}

Worker::~Worker()
{
	for (size_t i = 0; i < sockets.size(); i++)
		delete (sockets[i]);
}

void Worker::eventEVError(int k, struct kevent &event)
{
	// 서버 소켓 에러
	if (fd == sockets[k]->_serverFd)
		stderrExit("Server socket error");
	else
	{
		// 클라이언트 소켓 에러 아니면 다른 에러
		if (clients.find(fd) != clients.end())
		{
			sockets[k]->disconnectClient(fd, clients, event);
		}
	}
}

bool Worker::eventFilterRead(int k, struct kevent &event)
{
	if (!hasClientFd(k))
		return false;
	if (fd == sockets[k]->_serverFd)
	{
		int client_fd = sockets[k]->handleEvent(event_list);
		clients[client_fd].clear();
	}
	else if (clients.find(fd) != clients.end())
	{
		char buf[1024];
		int n = 1;
		while (0 < (n = recv(fd, buf, sizeof(buf) - 1, 0)))
		{
			buf[n] = '\0';
			clients[fd] += buf;
		}
		if (n < 1)
		{
			UData *uData = static_cast<UData *>(event.udata);
			if (uData->writeEventExist == false)
			{
				struct kevent new_event;
				uData->writeEventExist = true;
				EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, event.udata);
				event_list.push_back(new_event);
			}
		}

		// char buf[BUFFER_SIZE + 1];
		// ssize_t n;
		// struct kevent new_event;

		// while (true)
		// {
		// 	n = recv(fd, buf, BUFFER_SIZE, 0);
		// 	if (n < BUFFER_SIZE)
		// 	{
		// 		buf[n] = '\0';
		// 		clients[fd] += buf;
		// 		EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		// 		event_list.push_back(new_event);
		// 		std::cout << "read: " << clients[fd] << std::endl;
		// 		break;
		// 	}
		// 	else
		// 	{
		// 		// buf[BUFFER_SIZE] = '\0';
		// 		clients[fd] += buf;
		// 	}
	}
	return true;
}

bool Worker::eventFilterWrite(int k, struct kevent &event)
{
	if (!hasClientFd(k))
		return false;
	HTTPRequest *result = parser.parse(clients[fd]);
	if (!result)
		return false;
	// header가 존재하지 않는 경우 다시 요청 다시 받기 위함
	if (result->method != HEAD && result->headers.size() == 0)
		return false;
	if (result->port == -1)
		result->port = strtod(listen[0].value.c_str(), NULL);
	responseUData = static_cast<UData *>(event.udata);
	if (clients.find(fd) != clients.end() && result != NULL)
	{
		if (checkHeaderIsKeepLive(result))
			registerKeepAlive(result, event, fd);
		cookieCheck(result);
		if (responseUData->max == 0)
		{
			std::cout << "max is zero, disconnection!" << std::endl;
			sockets[k]->disconnectClient(fd, clients, event);
			return false;
		}
		if (result)
		{
			this->requestHandler(*result, fd);
			struct kevent eventToDelete;
			EV_SET(&eventToDelete, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
			kevent(kq, &eventToDelete, 1, NULL, 0, NULL);
			responseUData->writeEventExist = false;
		}
		else
			std::cout << "Failed to parse request" << std::endl;
		responseUData->max = responseUData->max - 1;
		// if (!checkHeaderIsKeepLive(result) || responseUData->max == 0)
		// sockets[k]->disconnectClient(fd, clients, event);
		clients[fd].clear();
	}
	if (result)
		delete result;

	return true;
}

bool Worker::eventEOF(int k, struct kevent &event)
{
	if (!hasClientFd(k))
		return false;
	std::cout << "client want to disconnect" << std::endl;
	sockets[k]->disconnectClient(fd, clients, event);
	return true;
}

bool Worker::eventFilterTimer(int k, struct kevent &event)
{
	if (!hasClientFd(k))
		return false;
	std::cout << fd << " is time over" << std::endl;
	deleteTimer(fd);
	sockets[k]->disconnectClient(fd, clients, event);
	return true;
}

void Worker::run()
{
	struct kevent events[10];
	struct kevent event;
	int nevents;

	config.getAllDirectives(this->listen, config.getDirectives(), "listen");
	int sockets_size;
	while (true)
	{
		nevents = kevent(kq, &event_list[0], event_list.size(), events, 10, NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		event_list.clear();
		sockets_size = sockets.size();
		for (int k = 0; k < sockets_size; k++)
		{
			for (int i = 0; i < nevents; i++)
			{
				event = events[i];
				fd = event.ident;

				if (event.flags & EV_ERROR)
					eventEVError(k, event);
				if (event.flags & EV_EOF)
				{
					if (eventEOF(k, event) == false)
						continue;
				}
				else if (event.filter == EVFILT_READ)
				{
					if (eventFilterRead(k, event) == false)
						continue;
				}
				else if (event.filter == EVFILT_WRITE)
				{
					if (eventFilterWrite(k, events[i]) == false)
						continue;
				}
				else if (event.filter == EVFILT_TIMER)
				{
					if (eventFilterTimer(k, event) == false)
						continue;
				}
				else if (event.filter == EVFILT_SIGNAL)
					signal.handleEvent(event, sockets);
			}
		}
	}
}

/*
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 서버의 fd
 */
void Worker::requestHandler(const HTTPRequest &request, const int &client_fd)
{
	Response res;
	ResponseData *response = res.getResponseData(request, client_fd, config, this->server);
	if (response->path == "/session" && responseUData->sessionID.empty() && responseUData->sesssionValid == false) // 만약 /session 으로 요청이 들어온다면 session을 만들어줌
		responseUData->sessionID = generateSessionID(32);
	else if (response->path == "/session/delete" && responseUData->alreadySessionSend == true &&
			 responseUData->sessionID != "")
	{
		responseUData->wantToDeleteSessionInCookie = true;
	}
	if (std::find(response->limitExcept.begin(), response->limitExcept.end(), request.method) == response->limitExcept.end()) // limitExcept에 method가 없는 경우
	{
		// 잘못된 메서드일경우
		std::cout << "Method not allowed" << std::endl;
		errorResponse(response, 405);
		delete response;
		return;
	}
	// 현재 메서드와 limit을 비교후 바로 404 갈지 실행한지 분기
	if (response->method == GET)
	{
		if (isCGIRequest(*response))
		{
			CGI cgi(request);
			std::string resource_content = cgi.excuteCGI(response->resourcePath);
			std::size_t tmpIdx = resource_content.find("\n\n");
			if (tmpIdx != std::string::npos)
				resource_content = resource_content.substr(tmpIdx + 2);
			response->resourcePath = getCGILocation(response);
			if (response->resourcePath.empty())
			{
				std::cout << "getCGILocation" << std::endl;
				errorResponse(response, 404);
				return;
			}
			std::string response_header = generateHeader(resource_content, "text/html", 200);
			ftSend(response, response_header);
			ftSend(response, resource_content);
			return;
		}
		getResponse(response);
	}
	else if (response->method == POST)
	{
		if (isCGIRequest(*response)) // TODO CGI도 client_max_body_size 적용해야하나?
		{
			// cgi post method 실행
			std::cout << request.query << std::endl;
			// std::cout << "YOUPI.BLA" << std::endl;
			std::cout << "POST PATH : " << response->resourcePath << std::endl;
			std::cout << "[" << request.body.length() << "]" << std::endl;
			CGI cgi(request);
			std::string resource_content = cgi.excuteCGI(response->resourcePath);
			std::size_t tmpIdx = resource_content.find("\r\n\r\n");
			if (tmpIdx != std::string::npos)
				resource_content = resource_content.substr(tmpIdx + 2);
			std::cout << "[" << resource_content.length() << "]" << std::endl;
			std::cout << "[" << resource_content.substr(0, 100) << "]" << std::endl;
			response->resourcePath = getCGILocation(response);
			if (response->resourcePath.empty())
			{
				std::cout << "postCGILocation" << std::endl;
				errorResponse(response, 404);
				return;
			}
			std::string response_header = generateHeader(resource_content, "text/html", 200);
			std::cout << "post1" << std::endl;
			ftSend(response, response_header);
			std::cout << "post2" << std::endl;
			ftSend(response, resource_content);
			std::cout << "post3" << std::endl;
			return;
		}
		// body size가 0인지 확인. body size가 0인 경우 GET 메소드와 다르지 않기 때문에 GET 메소드 실행함수로 리다이렉션해도 상관없습니다.
		if (response->contentLength == 0)
		{
			getResponse(response);
			return;
		}
		// 해당 서브젝트 수준에서는 리소스가 CGI가 아니라면 body가 있든 없든, query가 있든 없든 처리/응답에는 영향이 없습니다.
		postResponse(response);
	}
	else if (response->method == HEAD)
	{
		// HEAD 메소드는 GET 메소드와 동일하지만, body가 없습니다.
		// 따라서 GET 메소드 실행함수로 리다이렉션해도 상관없습니다.
		getResponse(response);
	}
	else if (response->method == PUT)
	{
		std::cout << "PUT HERE" << std::endl;
		putResponse(response);
	}
	else if (response->method == OPTIONS)
	{
		// OPTIONS 메소드는 서버가 지원하는 메소드를 확인하기 위한 메소드입니다.
		// 따라서 서버가 지원하는 메소드를 응답해주면 됩니다.
		std::string response_content = "GET, POST, HEAD, PUT, DELETE, OPTIONS";
		std::string response_header = generateHeader(response_content, "text/html", 200);
		ftSend(response, response_header);
		ftSend(response, response_content);
	}
	else if (response->method == DELETE)
	{
		deleteResponse(response);
	}
	else
		stderrExit("Unknown method");
	delete response;
}

/**
 * GET 요청 중 CGI일 경우, CGI 반환에 필요한 location을 찾아 경로 반환
 *
 * @param response 응답시 사용될 구조체
 * @return 경로 문자열
 */
std::string Worker::getCGILocation(ResponseData *response)
{
	std::vector<Directive> &locations = response->server.locations;
	for (size_t i = 0; i < locations.size(); ++i)
	{
		Directive &location = locations[i];
		if (location.value == "/result")
		{
			std::string root = "";
			std::string index = "";
			for (size_t j = 0; j < location.block.size(); j++)
			{
				Directive &block = location.block[j];
				if (block.name == "root")
					root = block.value;
				if (block.name == "index")
					index = block.value;
			}
			return root + "/" + index;
		}
	}
	return "";
}

bool Worker::isCGIRequest(ResponseData &response)
{
	// 이 부분은 CGI 요청을 확인하는 로직을 구현합니다.
	// 예를 들어, 요청 URL에 특정 확장자(.cgi, .php 등)가 포함되어 있는지 확인할 수 있습니다.
	// 요청이 CGI 요청인 경우 true를 반환하고, 그렇지 않은 경우 false를 반환합니다.
	if (response.location == NULL)
		return false;

	// const std::vector<Directive> &location = response.location->block;
	// for (std::vector<Directive>::const_iterator it = location.begin(); it != location.end(); it++)
	// {
	// 	Directive directive = *it;
	// 	std::cout << "A " << directive.name << std::endl;
	// 	if (directive.name == "cgi_path")
	// 	{
	// 		std::cout << "B ==" << directive.value << "==" << std::endl;
	// 		size_t pos = response.path.find(directive.value);
	// 		if (pos != std::string::npos)
	// 			return true;
	// 	}
	// }

	size_t pos = response.path.find(".", response.path.find_last_of("/"));
	if (pos == std::string::npos)
		return false;
	std::string tmp = response.path.substr(pos);
	if (std::find(response.cgiPath.begin(), response.cgiPath.end(), tmp) != response.cgiPath.end())
	{
		tmp = response.cgiPath.back();
		if (response.method == POST && tmp.find(".") == std::string::npos)
		{
			response.path = response.path.substr(0, response.path.find_last_of("/"));
			response.path += "/";
			response.path += tmp;
		}
		response.cgiPath.push_back(tmp);
		response.resourcePath = response.path;
		return true;
	}
	return false;
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param response 응답시 사용될 구조체
 */
void Worker::getResponse(ResponseData *response)
{
	if (invalidResponse(response))
		return;

	std::string resource_content = readFile(response->resourcePath);
	if (resource_content.empty())
		return errorResponse(response, 404);
	std::string response_header = generateHeader(resource_content, response->contentType, 200);
	ftSend(response, response_header);
	ftSend(response, resource_content);
}

void Worker::postResponse(ResponseData *response)
{
	const std::string clientMaxBodySize = "client_max_body_size";
	if (invalidResponse(response))
		return;

	// TODO PUT도 해야 하나?
	// check client_max_body_size
	std::vector<Directive>::const_iterator it = findDirective(response->location->block, clientMaxBodySize);
	if (it == response->location->block.end())
		it = findDirective(response->server.locations, clientMaxBodySize);
	if (it != response->location->block.end())
	{
		size_t max_body_size = atoi(it->value.c_str());
		if (max_body_size < response->contentLength)
			return errorResponse(response, 400);
	}
	writeFile(response->resourcePath, response->body);
	std::string body = ""; // POST는 생성된 내용을 반환하지 않아도 됨.
	std::string response_header = generateHeader(body, response->contentType, 201);
	ftSend(response, response_header);
}

void Worker::putResponse(ResponseData *response)
{
	// TODO 이거 경로 제대로 되게 해야 함. 임시임
	if (response->body.length() > 10000)
		response->body = response->body.substr(0, 10000);
	if (writeFile(response->resourcePath, response->body))
	{
		// 리소스 생성에 성공한 경우
		std::cout << "putResponse, response->resoursePath : " << response->resourcePath << std::endl;
		std::string resource_content = readFile(response->resourcePath);
		if (resource_content.empty())
			return errorResponse(response, 404);
		std::string resource_header = generateHeader(resource_content, "text/html", 201);
		ftSend(response, resource_header);
		ftSend(response, resource_content);
	}
	else
	{
		// 리소스 생성에 실패한 경우
		std::cout << "Failed to create the resource" << std::endl;
		errorResponse(response, 500);
	}
}

void Worker::deleteResponse(ResponseData *response)
{
	std::string resourcePath = response->resourcePath;

	if (remove(resourcePath.c_str()) != 0)
	{
		// 삭제에 실패한 경우
		std::cout << "Failed to delete the resource" << std::endl;
		errorResponse(response, 500);
	}
	else
	{
		// 삭제에 성공한 경우
		std::string response_content = "Resource deleted successfully";
		std::string response_header = generateHeader(response_content, "text/html", 200);
		ftSend(response, response_header);
		ftSend(response, response_content);
	}
}

/**
 * 에러 코드에 대한 페이지가 존재하지 않는 경우 페이지 새로 생성
 *
 * @param client_fd 브라우저 포트번호
 */
std::string Worker::errorPageGenerator(int errorCode)
{
	std::stringstream broadHtml;
	broadHtml << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"utf-8\">\n\t<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n\t<metaname=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>error page</title>\n</head>\n<body>\n\t<h1>This is " << errorCode << " Page.</h1>\n</body>\n</html>";
	return broadHtml.str();
}

/**
 * 모든 에러에 대한 에러 페이지를 띄워주는 함수
 *
 * @param client_fd 브라우저 포트번호
 */
void Worker::errorResponse(ResponseData *response, int errorCode)
{
	std::string errorContent;
	std::map<int, std::string>::iterator it = response->server.errorPage.find(errorCode);
	if (it == response->server.errorPage.end())
		errorContent = errorPageGenerator(errorCode);
	else
	{
		const std::string errorPath = response->root + it->second;
		errorContent = readFile(errorPath);
		if (errorContent == "")
			errorContent = errorPageGenerator(errorCode);
	}
	ftSend(response->clientFd, generateErrorHeader(errorCode, errorContent));
	ftSend(response->clientFd, errorContent);
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @param contentType Content-Type
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateHeader(const std::string &content, const std::string &contentType, int statusCode)
{
	HTTPRequestParser parser;
	std::ostringstream oss;

	oss << "HTTP/1.1 " << statusCode << " OK" << CRLF;
	oss << "Content-Length: " << content.length() << CRLF;
	oss << "Content-Type: " << contentType << CRLF; // MIME type can be changed as needed
	if (responseUData->alreadySessionSend == true &&
		responseUData->sessionID != "" &&
		responseUData->wantToDeleteSessionInCookie == true)
	{
		std::string expireTime = getExpiryDate(-3600);
		oss << "Set-Cookie: sessionid="
			<< "deleted"
			<< "; Expires=" << expireTime << "; Path=/" << CRLF;
		responseUData->alreadySessionSend = false;
		responseUData->expireTime = "";
		responseUData->wantToDeleteSessionInCookie = false;
		responseUData->sessionID = "";
	}
	else if (responseUData->alreadySessionSend == false && responseUData->sessionID != "")
	{
		std::string expireTime = getExpiryDate(3600);
		oss << "Set-Cookie: sessionid=" << responseUData->sessionID
			<< "; Expires=" << expireTime << "; Path=/" << CRLF;
		responseUData->alreadySessionSend = true;
		responseUData->expireTime = expireTime;
	}
	if (responseUData->keepLive)
		oss << "Connection: keep-alive" << CRLF2;
	else
		oss << "Connection: close" << CRLF2;
	return oss.str();
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param request request 를 파싱완료한 구조체
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateErrorHeader(int status_code, const std::string &message)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << status_code << " OK" << CRLF;
	oss << "Content-Length: " << message.length() << CRLF;
	oss << "Content-Type: text/html" << CRLF;
	;
	oss << "Connection: close" << CRLF2;
	return oss.str();
}

void Worker::broad(ResponseData *response)
{
	std::stringstream broadHtml;
	broadHtml << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>broad page</title></head><body><h1>show</h1>";
	DIR *dirPtr = NULL;
	dirent *file;
	// if ((dirPtr = opendir(response->root.c_str())) != NULL)
	if ((dirPtr = opendir(response->root.c_str())) == NULL)
	{
		std::cout << "broad: location path err" << std::endl;
		return;
	}
	while ((file = readdir(dirPtr)))
	{
		broadHtml << "<p><a href=" << response->locationName << "/" << file->d_name << ">" << file->d_name << "</a></p>";
	}
	broadHtml << "</body></html>";
	std::string tmp = broadHtml.str();
	/* 헤더를 작성해주는과정 */
	MimeTypesParser mime(config);
	std::string contentType = mime.getMimeType("html");
	std::string response_header = generateHeader(tmp, contentType, 200);
	ftSend(response, response_header);
	ftSend(response, tmp); // 완성된 html 을 body로 보냄
}

bool Worker::checkHeaderIsKeepLive(const HTTPRequest *request)
{
	std::map<std::string, std::string>::const_iterator it = request->headers.find("Connection");
	if (it != request->headers.end())
	{
		std::string value = it->second;
		if (value.length() != 0 && value[value.length() - 1] == '\r')
			value.erase(value.length() - 1);
		if (value == "keep-alive")
			return true;
		else
			return false;
	}
	return false;
}

bool Worker::checkKeepLiveOptions(const HTTPRequest *request, struct kevent &event)
{
	UData *uData = static_cast<UData *>(event.udata);
	// std::map<std::string, std::string>::const_iterator it = request->headers.find("keep-alive"); // 표준이지만, modHeader 익스텐션에서는 아래로 써야함.
	std::map<std::string, std::string>::const_iterator it = request->headers.find("keep-alive");
	std::string timeout;
	std::string max;
	size_t timeoutIdx;
	size_t maxIdx;
	if (it != request->headers.end())
	{
		std::string value = it->second;
		if (value.length() != 0 && value[value.length() - 1] == '\r')
			value.erase(value.length() - 1);
		std::vector<std::string> options = Config::split(value, ',');
		if (options.size() != 1 && options.size() != 2)
			return false;
		for (size_t i = 0; i < options.size(); i++)
		{
			timeoutIdx = options[i].find("timeout=");
			maxIdx = options[i].find("max=");
			if (timeoutIdx == std::string::npos && maxIdx == std::string::npos)
				return false;
			if (timeoutIdx != std::string::npos)
			{
				timeout = options[i].substr(timeoutIdx + 8, options[i].length() - 1);
				if (timeout.find_first_not_of("0123456789") != std::string::npos)
					return false;
				uData->timeout = std::stoi(timeout.c_str());
				if (uData->timeout < 0)
					return false;
			}
			if (maxIdx != std::string::npos)
			{
				max = options[i].substr(maxIdx + 4, options[i].length() - 1);
				if (max.find_first_not_of("0123456789") != std::string::npos)
					return false;
				uData->max = std::stoi(max.c_str());
				if (uData->max < 0)
					return false;
			}
		}
		return true;
	}
	return false;
}

void Worker::setTimer(int fd, int timeout)
{
	struct kevent timerEvent;
	int timer_interval_ms = timeout * 1000;
	EV_SET(&timerEvent, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, timer_interval_ms, 0);
	kevent(kq, &timerEvent, 1, NULL, 0, NULL);
}

void Worker::deleteTimer(int fd)
{
	struct kevent timerEvent;
	EV_SET(&timerEvent, fd, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
	kevent(kq, &timerEvent, 1, NULL, 0, NULL);
}

void Worker::registerKeepAlive(const HTTPRequest *request, struct kevent &event, int client_fd)
{
	if (event.udata == NULL)
		return;
	UData *uData = static_cast<UData *>(event.udata);
	if (uData->keepLive == false)
	{
		uData->keepLive = true;
		if (checkKeepLiveOptions(request, event))
		{
			if (uData->timeout > 0)
				setTimer(client_fd, uData->timeout);
		}
		Socket::enableKeepAlive(client_fd);
	}
}

std::string Worker::getExpiryDate(int secondsToAdd)
{
	std::time_t now = std::time(0);
	std::tm *expiration = std::localtime(&now);
	expiration->tm_sec += secondsToAdd;
	std::mktime(expiration);
	char buffer[80];
	std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", expiration);
	return std::string(buffer);
}

void Worker::cookieCheck(HTTPRequest *result)
{
	if (result->headers.find("Cookie") != result->headers.end())
	{
		std::string cookie = result->headers["Cookie"];
		if (cookie.find("sessionid="))
		{
			std::string cookieSessionId = cookie.substr(10, 42);
			if (responseUData->sessionID == cookieSessionId)
				responseUData->sesssionValid = true;
			else
				responseUData->sesssionValid = false;
		}
		responseUData->sesssionValid = isCookieValid(responseUData->expireTime);
		if (responseUData->sesssionValid)
			std::cout << "session is valid" << std::endl;
		else
			std::cout << "session is invalid" << std::endl;
	}
	else
		responseUData->sesssionValid = false;
}

bool Worker::isCookieValid(const std::string &expireTime)
{
	std::tm expirationTime = {};
	std::istringstream iss(expireTime);
	iss >> std::get_time(&expirationTime, "%a, %d %b %Y %H:%M:%S");
	if (iss.fail())
		return false;
	std::time_t currentTime = std::time(0);
	std::time_t expiration = std::mktime(&expirationTime);
	if (currentTime >= expiration)
		return false;
	return true;
}

std::string Worker::generateSessionID(int length)
{
	const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string sessionID;
	std::srand(std::time(0));
	for (int i = 0; i < length; i++)
	{
		int index = std::rand() % charset.length();
		sessionID += charset[index];
	}
	return sessionID;
}

void Worker::redirection(ResponseData *response)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << response->returnState << " ok" << CRLF;
	oss << "Location: " << response->redirect << CRLF;
	oss << "Connection: close" << CRLF2;
	ftSend(response->clientFd, oss.str());
	return;
}

bool Worker::invalidResponse(ResponseData *response)
{
	if (!isFile(response->resourcePath))
	{
		if (response->autoindex)
			broad(response);
		else if (!response->redirect.empty())
			redirection(response);
		else
			errorResponse(response, 404);
		return true;
	}
	return false;
}

bool Worker::hasClientFd(const int &k)
{
	Socket *socket = sockets[k];
	std::vector<int>::iterator it = std::find(socket->_clientFds.begin(), socket->_clientFds.end(), fd);
	if (it == socket->_clientFds.end())
		return false;
	return true;
}
