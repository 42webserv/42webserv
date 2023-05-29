/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/29 15:19:55 by chanwjeo         ###   ########.fr       */
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
			sockets.push_back(new Socket(master.getEvents(), server.servers[i].ports[j], kq));
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
		errorExit("Server socket error");
	else
	{
		// 클라이언트 소켓 에러 아니면 다른 에러
		if (clients.find(fd) != clients.end())
			sockets[k]->disconnectClient(fd, clients, event);
	}
}

bool Worker::eventFilterRead(int k, struct kevent &event)
{
	found = std::find(sockets[k]->_clientFds.begin(), sockets[k]->_clientFds.end(), fd);
	if (found == sockets[k]->_clientFds.end())
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
		while (0 < (n = read(fd, buf, sizeof(buf))))
		{
			buf[n] = '\0';
			clients[fd] += buf;
		}
		if (n < 1)
		{
			// HTML 요청 메세지 보기
			// struct kevent new_event;
			// EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			// event_list.push_back(new_event);
			// std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;
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
	found = std::find(sockets[k]->_clientFds.begin(), sockets[k]->_clientFds.end(), fd);
	if (found == sockets[k]->_clientFds.end())
		return false;
	HTTPRequest *result = parser.parse(clients[fd]);
	if (!result)
	{
		return false;
	}
	// header가 존재하지 않는 경우 다시 요청 다시 받기 위함
	if (result->method != "HEAD" && result->headers.size() == 0)
		return false;
	if (result->port == -1)
		result->port = strtod(listen[0].value.c_str(), NULL);
	UData *uData = static_cast<UData *>(event.udata);
	if (clients.find(fd) != clients.end() && result != NULL)
	{
		if (checkHeaderIsKeepLive(result))
			registerKeepAlive(result, event, fd);
		if (uData->max == 0)
		{
			std::cout << "max is zero, disconnection!" << std::endl;
			sockets[k]->disconnectClient(fd, clients, event);
			return false;
		}
		if (result)
		{
			this->requestHandler(*result, fd);
		}
		else
			std::cout << "Failed to parse request" << std::endl;
		uData->max = uData->max - 1;
		// if (uData->max > 0)
		// 	std::cout << "max = " << uData->max << std::endl;
		if (!checkHeaderIsKeepLive(result) || uData->max == 0)
			sockets[k]->disconnectClient(fd, clients, event);
		clients[fd].clear();
	}
	if (result)
		delete result;
	return true;
}

bool Worker::eventEOF(int k, struct kevent &event)
{
	found = std::find(sockets[k]->_clientFds.begin(), sockets[k]->_clientFds.end(), fd);
	if (found == sockets[k]->_clientFds.end())
		return false;
	std::cout << "client want to disconnect" << std::endl;
	sockets[k]->disconnectClient(fd, clients, event);
	return true;
}

bool Worker::eventFilterTimer(int k, struct kevent &event)
{
	found = std::find(sockets[k]->_clientFds.begin(), sockets[k]->_clientFds.end(), fd);
	if (found == sockets[k]->_clientFds.end())
		return false;
	std::cout << fd << " is time over" << std::endl;
	deleteTimer(fd);
	sockets[k]->disconnectClient(fd, clients, event);
	return true;
}

int Worker::findSocketIndex(struct kevent &event) // 안필요할 것 같은데 일단 남겨둠.
{
	int k;

	k = 0;
	for (size_t j = 0; j < sockets.size(); j++)
	{
		if (sockets[j]->findClientFd(event.ident) == true)
		{
			k = j;
			return (k);
		}
		k = j;
	}
	std::cout << "fd is not exist" << std::endl;
	return (-1);
}

void Worker::run()
{
	struct kevent events[10];
	struct kevent event;
	int nevents;

	config.getAllDirectives(this->listen, config.getDirectives(), "listen");
	// std::cout << "listen : " << this->listen[0].value << std::endl;

	int sockets_size;
	// int k;
	while (true)
	{
		// std::cout << "here" << std::endl;
		nevents = kevent(kq, &event_list[0], event_list.size(), events, 10, NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		event_list.clear(); // 이벤트가 발생하면 event_list를 비워줌 왜>.?????
		sockets_size = sockets.size();
		for (int k = 0; k < sockets_size; k++)
		{
			for (int i = 0; i < nevents; i++)
			{
				// std::cout << "filter : " << event.filter << std::endl;
				// if ((k = findSocketIndex(events[i]) == -1))
				// {
				// 	std::cout << k << " : not exist" << std::endl;
				// 	continue;
				// }
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
void Worker::requestHandler(const HTTPRequest &request, int client_fd)
{
	Response res;
	ResponseData *response = res.getResponseData(request, client_fd, config, this->server);
	if (std::find(response->limitExcept.begin(), response->limitExcept.end(), request.method) == response->limitExcept.end()) // limitExcept에 method가 없는 경우
	{
		// 현재는 location을 찾지못해 limit.except에서 판별이안되 넘어오는 경우도있음!
		// 잘못된 메서드일경우
		// method not allowed
		std::string response_content = "Method not allowed";
		std::string response_header = generateErrorHeader(405, response_content);
		ftSend(response, response_header);
		ftSend(response, response_content);
		delete response;
		return;
	}
	// 현재 메서드와 limit을 비교후 바로 404 갈지 실행한지 분기
	if (response->method == "GET")
	{
		if (isCGIRequest(response))
		{
			CGI cgi(request);
			std::string resource_content = cgi.excuteCGI(response->resourcePath, request);
			if ((response->resourcePath = getCGILocation(response)) == "")
			{
				std::cout << "getLocation" << std::endl;
				// error_page
				return;
			}
			std::ifstream resource_file(response->resourcePath);
			std::string response_header = generateHeader(resource_content, "text/html", 200);
			ftSend(response, response_header);
			ftSend(response, resource_content);
			resource_file.close();
			return;
		}
		getResponse(response);
	}
	else if (response->method == "POST")
	{
		// TODO 이건 뭘지 확인하기.
		if (response->contentLength == 0)
		{
			std::cout << "here" << std::endl;
			delete response;
			return;
		}
		if (isCGIRequest(response))
		{
			// cgi post method 실행
			std::cout << "YOUPI.BLA" << std::endl;
		}
		// body size가 0인지 확인. body size가 0인 경우 GET 메소드와 다르지 않기 때문에 GET 메소드 실행함수로 리다이렉션해도 상관없습니다.
		if (request.body.length() == 0)
		{
			getResponse(response);
			return;
		}
		// 해당 서브젝트 수준에서는 리소스가 CGI가 아니라면 body가 있든 없든, query가 있든 없든 처리/응답에는 영향이 없습니다.
		postResponse(response);
	}
	else if (response->method == "HEAD")
	{
		// HEAD 메소드는 GET 메소드와 동일하지만, body가 없습니다.
		// 따라서 GET 메소드 실행함수로 리다이렉션해도 상관없습니다.
		getResponse(response);
	}
	else if (response->method == "PUT")
	{
		// std::cout << "PUT" << std::endl;
		// temp
		std::string resourcePath = response->resourcePath.substr(0, response->resourcePath.find_last_of('/'));
		resourcePath += response->path.substr(response->path.find_last_of('/'));
		// std::cout << "resourcepath(PUT)" << resourcePath << std::endl;

		std::ofstream outFile(resourcePath, std::ios::out | std::ios::trunc);
		if (outFile.is_open())
		{
			outFile << response->body;
			outFile.close();
			// 리소스 생성에 성공한 경우
			std::ifstream resource_file(resourcePath);
			if (!resource_file.is_open())
				return errorResponse(response->clientFd);
			std::string resource_content((std::istreambuf_iterator<char>(resource_file)), std::istreambuf_iterator<char>());
			std::string resource_header = generateHeader(resource_content, "text/html", 201);
			ftSend(response, resource_header);
			ftSend(response, resource_content);
			resource_file.close();
		}
		else
		{
			// 리소스 생성에 실패한 경우
			std::string response_content = "Failed to create the resource";
			std::string response_header = generateErrorHeader(500, "text/html");
			ftSend(response, response_header);
			ftSend(response, response_content);
		}
	}
	else if (response->method == "OPTIONS")
	{
		// OPTIONS 메소드는 서버가 지원하는 메소드를 확인하기 위한 메소드입니다.
		// 따라서 서버가 지원하는 메소드를 응답해주면 됩니다.
		std::string response_content = "GET, POST, HEAD, PUT, DELETE, OPTIONS";
		std::string response_header = generateHeader(response_content, "text/html", 200);
		ftSend(response, response_header);
		ftSend(response, response_content);
	}
	else if (response->method == "DELETE")
	{
		std::string resourcePath = response->resourcePath;
		std::cout << "resourcepath(DELETE)" << resourcePath << std::endl;
		// 리소스 삭제 로직을
		if (remove(resourcePath.c_str()) != 0)
		{
			// 삭제에 실패한 경우
			std::string response_content = "Failed to delete the resource";
			std::string response_header = generateErrorHeader(500, "text/html");
			ftSend(response, response_header);
			ftSend(response, response_content);
		}
		else
		{
			// 삭제에 성공한 경우
			std::string response_content = "Resource deleted successfully";
			std::ifstream resource_file(response->resourcePath);
			std::string response_header = generateHeader(response_content, "text/html", 200);
			ftSend(response, response_header);
			ftSend(response, response_content);
			resource_file.close();
			return;
		}
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
	for (size_t i = 0; i < response->server.locations.size(); ++i)
	{
		if (response->server.locations[i].value == "/result ")
		{
			std::string root = "";
			std::string index = "";
			for (size_t j = 0; j < response->server.locations[i].block.size(); j++)
			{
				if (response->server.locations[i].block[j].name == "root")
					root = response->server.locations[i].block[j].value;
				if (response->server.locations[i].block[j].name == "index")
					index = response->server.locations[i].block[j].value;
			}
			return root + "/" + index;
		}
	}
	return "";
}

bool Worker::isCGIRequest(ResponseData *response)
{
	// 이 부분은 CGI 요청을 확인하는 로직을 구현합니다.
	// 예를 들어, 요청 URL에 특정 확장자(.cgi, .php 등)가 포함되어 있는지 확인할 수 있습니다.
	// 요청이 CGI 요청인 경우 true를 반환하고, 그렇지 않은 경우 false를 반환합니다.
	// return request.find(".py") != std::string::npos;
	size_t pos = response->path.find("cgi-bin");
	if (pos == std::string::npos && response->method == "POST")
		pos = response->path.find(".bla");
	return (pos != std::string::npos);
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param response 응답시 사용될 구조체
 */
void Worker::getResponse(ResponseData *response)
{
	struct stat st;
	// if (!stat(response->resourcePath.c_str(), &st)) // 파일인지 디렉토리인지 검사하기위해 stat함수 사용
	// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
	stat(response->resourcePath.c_str(), &st);
	if (!S_ISREG(st.st_mode)) // root + index을 검사해 파일이 아닐시 if로 분기
	{
		response->resourcePath = response->root + response->path; // root + path로 다시 검사
		std::memset(&st, 0, sizeof(st));
		// if (!stat(response->resourcePath.c_str(), &st))
		// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
		stat(response->resourcePath.c_str(), &st);
		if (!S_ISREG(st.st_mode))
		{
			if (response->autoindex)
				return broad(response);
			else
				return errorResponse(response->clientFd);
		}
	}
	std::ifstream resource_file(response->resourcePath); // 위에서 stat함수로 파일검사는 완료
	if (!resource_file.is_open())						 // 혹시 open이 안될수있으니 한번더 체크
		return errorResponse(response->clientFd);

	std::string resource_content((std::istreambuf_iterator<char>(resource_file)), std::istreambuf_iterator<char>());
	std::string response_header = generateHeader(resource_content, response->contentType, 200);
	ftSend(response, response_header);
	ftSend(response, resource_content);
	resource_file.close();
}

void Worker::postResponse(ResponseData *response) // request body 추가하기
{
	// request 사용?
	struct stat st;
	// if (!stat(response->resourcePath.c_str(), &st)) // 파일인지 디렉토리인지 검사하기위해 stat함수 사용
	// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
	stat(response->resourcePath.c_str(), &st);
	if (!S_ISREG(st.st_mode)) // root + index을 검사해 파일이 아닐시 if로 분기
	{
		response->resourcePath = response->root + response->path; // root + path로 다시 검사
		std::memset(&st, 0, sizeof(st));
		// if (!stat(response->resourcePath.c_str(), &st))
		// 	std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
		stat(response->resourcePath.c_str(), &st);
		if (!S_ISREG(st.st_mode))
		{
			if (response->autoindex)
				return broad(response);
			else
				return errorResponse(response->clientFd);
		}
	}
	std::ofstream outFile(response->resourcePath, std::ios::out | std::ios::trunc);
	outFile << response->body;
	outFile.close();

	std::ifstream inFile(response->resourcePath); // 위에서 stat함수로 파일검사는 완료
	if (!inFile.is_open())						  // 혹시 open이 안될수있으니 한번더 체크
		return errorResponse(response->clientFd);

	// POST는 생성된 내용을 반환하지 않아도 됨.
	std::string body = "";
	std::string response_header = generateHeader(body, response->contentType, 201);
	ftSend(response, response_header);
	// ftSend(response, resource_content);
	inFile.close();
}

/**
 * 404 에러일 경우 나와야할 페이지 띄워주는 함수
 *
 * @param client_fd 브라우저 포트번호
 */
void Worker::errorResponse(int client_fd)
{
	std::string error_path = "./assets/html/404.html";
	std::ifstream error_file(error_path);
	std::string error_content((std::istreambuf_iterator<char>(error_file)), std::istreambuf_iterator<char>());
	std::string error_header = generateErrorHeader(404, error_content);
	ftSend(client_fd, error_header);
	ftSend(client_fd, error_content);
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
	oss << "Connection: keep-alive" << CRLF2;
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
	// oss << "HTTP/1.1 " << status_code << " " << message << CRLF;
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
