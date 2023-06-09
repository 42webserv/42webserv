/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/09 17:48:32 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Master.hpp"
#include "Worker.hpp"

Worker::Worker(Master &master) : kq(master.kq), signal(master.getEvents()), config(master.getConfig()), events(master.getEvents()), server(master.getServer()) {}

Worker::~Worker() {}

void Worker::eventEVError(Socket &socket, struct kevent &event)
{
	const int &fd = event.ident;

	if (fd == socket._serverFd)
		stderrExit("Server socket error"); // 서버 소켓 에러
	else
		socket.disconnectClient(event);
}

void Worker::eventFilterSignal(struct kevent &event)
{
	signal.handleEvent(event.ident, server.servers);
}

void Worker::eventFilterRead(Socket &socket, struct kevent &event)
{
	const int &fd = event.ident;

	// Server socket
	if (fd == socket._serverFd)
	{
		socket.connectClient(events);
		return;
	}
	else
	{
		// Client socket
		socket.receiveRequest(event);
		UData *udata = static_cast<UData *>(event.udata);
		// Parse request
		HTTPRequest *result = parser.parse(udata->request);
		if (!result)
			return;
		udata->result = result;
		// Add write event
		struct kevent newEvent;
		EV_SET(&newEvent, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, udata);
		events.push_back(newEvent);
	}
}

void Worker::eventFilterWrite(Socket &socket, struct kevent &event)
{
	const int &fd = event.ident;
	UData *udata = static_cast<UData *>(event.udata);

	if (checkHeaderIsKeepLive(udata))
		registerKeepAlive(udata, fd);
	cookieCheck(udata);
	if (udata->max == 0) // TODO 테스트해보기
	{
		std::cout << "max is zero, disconnection!" << std::endl;
		socket.disconnectClient(event);
	}
	if (udata->result)
	{
		requestHandler(udata, fd);
		udata->request.clear();
		if (udata->keepLive == true)
			udata->max -= 1;
	}
	if (udata->max == 0)
	{
		std::cout << "communication number of " << fd << " is zero, disconnection!" << std::endl;
		socket.disconnectClient(event);
	}
}

void Worker::eventEOF(Socket &socket, struct kevent &event)
{
	const int &fd = event.ident;

	if (fd == socket._serverFd)
		stderrExit("Server socket EOF");
	else
		socket.disconnectClient(event);
}

void Worker::eventFilterTimer(Socket &socket, struct kevent &event)
{
	const int &fd = event.ident;

	std::cout << fd << " is time over" << std::endl;
	socket.disconnectClient(event);
}

void Worker::run()
{
	struct kevent eventList[10];
	struct kevent event;
	int nevents;

	memset(eventList, 0, sizeof(eventList));
	memset(&event, 0, sizeof(event));
	while (true)
	{
		nevents = kevent(kq, &events[0], events.size(), eventList, sizeof(eventList) / sizeof(eventList[0]), NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		events.clear();
		for (int i = 0; i < nevents; i++)
		{
			event = eventList[i];
			uintptr_t &fd = event.ident;
			Socket *socket = this->server.findSocket(fd);

			if (event.flags & EV_ERROR)
				eventEVError(*socket, event);
			else if (event.flags & EV_EOF)
				eventEOF(*socket, event);
			else
			{
				if (event.filter == EVFILT_READ)
					eventFilterRead(*socket, event);
				else if (event.filter == EVFILT_WRITE)
					eventFilterWrite(*socket, event);
				else if (event.filter == EVFILT_TIMER)
					eventFilterTimer(*socket, event);
				else if (event.filter == EVFILT_SIGNAL)
					eventFilterSignal(event);
			}
		}
	}
}

bool Worker::checkHttpRequestClientMaxBodySize(const HTTPRequest &request, ResponseData *response)
{
	std::map<std::string, std::string>::const_iterator it = request.headers.find("content-length");
	if (it != request.headers.end()) // Client의 request body size가 너무 큰 경우
	{
		std::string str = it->second.substr(0, it->second.find_last_of('\r'));
		std::stringstream ss(str);
		size_t requestBodySize;
		ss >> requestBodySize;

		size_t clientMaxBodySize = response->server.clientMaxBodySize;
		std::vector<Directive>::const_iterator dir = Utils::findDirectiveNameValue(response->server.locations, LOCATION_DIRECTIVE, request.path);
		if (dir != response->server.locations.end())
		{
			std::vector<Directive>::const_iterator dirr;
			dirr = Utils::findDirective(dir->block, CLIENT_MAX_BODY_SIZE_DIRECTIVE);
			if (dirr != dir->block.end())
				clientMaxBodySize = atoi(dirr->value.c_str());
		}
		if (requestBodySize > clientMaxBodySize)
		{
			std::cout << "It has too big body than client_max_body_size" << std::endl;
			errorResponse(response, 413);
			return false;
		}
	}
	return true;
}

/*
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param clientFd 서버의 fd
 */
void Worker::requestHandler(UData *udata, const int &clientFd)
{
	const HTTPRequest &request = *udata->result;
	Response res;
	ResponseData *response = res.getResponseData(request, clientFd, config, this->server);
	response->udata = udata;
	if (std::find(response->limitExcept.begin(), response->limitExcept.end(), request.method) == response->limitExcept.end()) // limitExcept에 method가 없는 경우
	{
		// 잘못된 메서드일경우
		std::cout << "Method not allowed" << std::endl;
		errorResponse(response, 405);
		delete response;
		return;
	}

	// /cgi-bin/printEnvp -> /cgi-bin/printEnvp.py로 변경해줘야 404 안걸림
	if (isCGIRequest(*response))
		response->resourcePath = getCGIPath(*response);

	if (checkHttpRequestClientMaxBodySize(request, response) == false || invalidResponse(response))
	{
		delete response;
		return;
	}
	if (response->location->value == "/session" && udata->sessionID.empty() && udata->sesssionValid == false) // 만약 /session 으로 요청이 들어온다면 session을 만들어줌
		udata->sessionID = generateSessionID(32);
	else if (response->location->value == "/session/delete" && udata->alreadySessionSend == true &&
			 udata->sessionID != "")
		udata->wantToDeleteSessionInCookie = true;

	// 메서드에 따른 응답처리
	if (response->method == GET || response->method == POST || response->method == HEAD) // TODO DELETE도 처리해주나
		sendResponse(response, request);
	else if (response->method == PUT)
	{
		putResponse(response);
	}
	else if (response->method == OPTIONS)
	{
		// OPTIONS 메소드는 서버가 지원하는 메소드를 확인하기 위한 메소드입니다.
		// 따라서 서버가 지원하는 메소드를 응답해주면 됩니다.
		std::string response_content = "GET, POST, HEAD, PUT, DELETE, OPTIONS";
		// std::string response_header = generateHeader(response_content, "text/html", 200, false);
		std::string response_header = generateHeader(response_content, "text/html", 200, response);
		Utils::ftSend(response, response_header);
		Utils::ftSend(response, response_content);
	}
	else if (response->method == DELETE)
	{
		deleteResponse(response);
	}
	else
		stderrExit("Unknown method");
	delete response;
}

void Worker::sendResponse(ResponseData *response, const HTTPRequest &request)
{
	std::string resourceContent;
	std::string content;
	std::string chunkData;

	// cgi 세팅
	CGI cgi(request);

	// X-HEADER 세팅
	std::map<std::string, std::string>::iterator XHeaderIterator = response->headers.find("X-Secret-Header-For-Test");
	if (XHeaderIterator != response->headers.end())
		cgi.setEnvp("HTTP_X_SECRET_HEADER_FOR_TEST", XHeaderIterator->second);

	if (isCGIRequest(*response))
	{
		setResponse(response, cgi.executeCGI(getCGIPath(*response)));
		resourceContent = response->body;
		Utils::ftSend(response, generateHeader(resourceContent, response->contentType, response->statusCode, response));
	}
	else
	{
		std::string path;
		size_t pos = response->path.rfind("/"); // "./src/cgi-bin/upload.py" -> upload.py 추출
		if (pos != std::string::npos)
			path = response->path.substr(pos + 1);
		if (path == "upload")
			return;
		if (response->method == POST)
		{
			resourceContent = response->body;
			Utils::writeFile(response->resourcePath, resourceContent);
		}
		else
			resourceContent = Utils::readFile(response->resourcePath);
		Utils::ftSend(response, generateHeader(resourceContent, response->contentType, 201, response));
	}
	if (response->chunked)
	{
		size_t streamSize = (resourceContent.length() / CHUNK_SIZE * CHUNK_SIZE == resourceContent.length()) ? resourceContent.length() / CHUNK_SIZE : resourceContent.length() / CHUNK_SIZE + 1;
		for (size_t contentIndex = 0; contentIndex < streamSize; contentIndex++)
		{
			if (contentIndex == streamSize - 1)
				content = resourceContent.substr(contentIndex * CHUNK_SIZE, resourceContent.length() - contentIndex * CHUNK_SIZE);
			else
				content = resourceContent.substr(contentIndex * CHUNK_SIZE, CHUNK_SIZE);
			chunkData = Utils::toHexString(content.length()) + "\r\n" + content + "\r\n";
			Utils::ftSend(response, chunkData);
		}
		Utils::ftSend(response, "0\r\n\r\n");
	}
	else
		Utils::ftSend(response, resourceContent);
}

/**
 * cgi에 해당할 때, 로케이션 상태에 따라 CGI path를 따로 설정해주는 함수
 *
 * @param CGIPath executeCGI 함수에 사용될 문자열
 */
std::string Worker::getCGIPath(ResponseData &response)
{
	for (size_t i = 0; i < response.location->block.size(); i++)
		if (response.location->block[i].name == "root")
		{
			if (response.cgiPath.size() == 1)
			{
				size_t pos = response.path.find(".", response.path.find_last_of("/"));
				if (pos == std::string::npos)
					return response.location->block[i].value + response.path.substr(response.path.find_last_of("/")) + response.cgiPath.back();
				else
					return response.location->block[i].value + response.path.substr(response.path.find_last_of("/"));
			}
			else
				return response.location->block[i].value + "/" + response.cgiPath.back();
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

	// /cgi_bin 로케이션을 위함
	if (response.cgiPath.size() == 1)
	{
		// upload
		std::string path;
		size_t pos = response.path.rfind("/"); // "./src/cgi-bin/upload.py" -> upload.py 추출
		if (pos != std::string::npos)
			path = response.path.substr(pos + 1);
		std::cout << "path " << path << std::endl; // upload.py
		// ./src/cgi-bin/src/cgi-bin/upload.py
		if (path == "upload") // uploadFile
		{
			std::string uploadContent = Utils::uploadPageGenerator("/cgi-bin/upload.py"); // root + upload + .py
			// std::string response_header = generateHeader(uploadContent, "text/html", 200, false);
			std::string response_header = generateHeader(uploadContent, "text/html", 200, &response);
			Utils::ftSend(response, response_header);
			Utils::ftSend(response, uploadContent);
			return false;
		}
		return true;
	}

	size_t pos = response.path.find(".", response.path.find_last_of("/"));
	if (pos == std::string::npos)
		return false;
	std::string tmp = response.path.substr(pos);
	if (std::find(response.cgiPath.begin(), response.cgiPath.end(), tmp) != response.cgiPath.end())
		return true;
	return false;
}

void Worker::putResponse(ResponseData *response)
{
	if (response->body.length() > 10000)
		response->body = response->body.substr(0, 10000);
	if (Utils::writeFile(response->resourcePath, response->body))
	{
		// 리소스 생성에 성공한 경우
		std::string resource_content = Utils::readFile(response->resourcePath);
		if (resource_content.empty())
			return errorResponse(response, 404);
		// std::string resource_header = generateHeader(resource_content, "text/html", 201, false);
		response->chunked = false;
		std::string resource_header = generateHeader(resource_content, "text/html", 201, response);
		Utils::ftSend(response, resource_header);
		Utils::ftSend(response, resource_content);
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
	// std::string resourcePath = response->resourcePath;
	std::string resourcePath = response->path;
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
		// std::string response_header = generateHeader(response_content, "text/html", 200, false);
		std::string response_header = generateHeader(response_content, "text/html", 200, response);
		Utils::ftSend(response, response_header);
		Utils::ftSend(response, response_content);
	}
}

/**
 * 모든 에러에 대한 에러 페이지를 띄워주는 함수
 *
 * @param response 응답에 사용될 구조체
 * @param errorCode 에러 코드
 */
void Worker::errorResponse(ResponseData *response, int errorCode)
{
	std::string errorContent;
	std::map<int, std::string>::iterator it = response->server.errorPage.find(errorCode);
	if (it == response->server.errorPage.end())
		errorContent = Utils::errorPageGenerator(response, errorCode);
	else
	{
		const std::string errorPath = response->server.root + it->second;
		errorContent = Utils::readFile(errorPath);
		if (errorContent == "")
			errorContent = Utils::errorPageGenerator(response, errorCode);
	}
	response->chunked = false;
	Utils::ftSend(response->clientFd, generateHeader(errorContent, "text/html", errorCode, response));
	Utils::ftSend(response->clientFd, errorContent);
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @param contentType Content-Type
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateHeader(const std::string &content, const std::string &contentType, int statusCode, ResponseData *response)
{
	std::ostringstream oss;

	UData *udata = response->udata;
	oss << "HTTP/1.1 " << statusCode << " " << response->statusCodeMap[statusCode] << CRLF;
	oss << "Content-Type: " << contentType << CRLF; // MIME type can be changed as needed
	if (response->chunked)
		oss << "Transfer-Encoding: chunked" << CRLF;
	else
		oss << "Content-Length: " << content.length() << CRLF;
	if (udata->alreadySessionSend == true && udata->sessionID != "" && udata->wantToDeleteSessionInCookie == true)
	{
		std::string expireTime = Utils::getExpiryDate(-3600);
		oss << "Set-Cookie: sessionid="
			<< "deleted"
			<< "; Expires=" << expireTime << "; Path=/" << CRLF;
		udata->alreadySessionSend = false;
		udata->expireTime = "";
		udata->wantToDeleteSessionInCookie = false;
		udata->sessionID = "";
	}
	else if (udata->alreadySessionSend == false && udata->sessionID != "")
	{
		std::string expireTime = Utils::getExpiryDate(3600);
		oss << "Set-Cookie: sessionid=" << udata->sessionID
			<< "; Expires=" << expireTime << "; Path=/" << CRLF;
		udata->alreadySessionSend = true;
		udata->expireTime = expireTime;
	}
	if (udata->keepLive)
		oss << "Connection: keep-alive" << CRLF2;
	else
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
	// std::string response_header = generateHeader(tmp, contentType, 200, false);
	std::string response_header = generateHeader(tmp, contentType, 200, response);
	Utils::ftSend(response, response_header);
	Utils::ftSend(response, tmp); // 완성된 html 을 body로 보냄
}

bool Worker::checkHeaderIsKeepLive(UData *udata)
{
	HTTPRequest *request = udata->result;
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

bool Worker::checkKeepLiveOptions(UData *udata)
{
	HTTPRequest *request = udata->result;
	std::map<std::string, std::string>::const_iterator it = request->headers.find("keep-alive"); // 표준이지만, modHeader 이걸로
																								 // std::map<std::string, std::string>::const_iterator it = request->headers.find("Keep-Alive");
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
				udata->timeout = Utils::ftStoi(timeout);
				if (udata->timeout < 0)
					return false;
			}
			if (maxIdx != std::string::npos)
			{
				max = options[i].substr(maxIdx + 4, options[i].length() - 1);
				if (max.find_first_not_of("0123456789") != std::string::npos)
					return false;
				udata->max = Utils::ftStoi(max);
				if (udata->max < 0)
					return false;
			}
		}
		return true;
	}
	return false;
}

void Worker::registerKeepAlive(UData *udata, int clientFd)
{
	if (udata->keepLive == false)
	{
		udata->keepLive = true;
		if (checkKeepLiveOptions(udata))
		{
			if (udata->timeout > 0)
				Utils::setTimer(kq, clientFd, udata->timeout);
		}
		Socket::enableKeepAlive(clientFd);
	}
}

void Worker::cookieCheck(UData *udata)
{
	HTTPRequest *request = udata->result;
	if (request->headers.find("Cookie") != request->headers.end())
	{
		std::string cookie = request->headers["Cookie"];
		if (cookie.find("sessionid="))
		{
			std::string cookieSessionId = cookie.substr(10, 42);
			if (udata->sessionID == cookieSessionId)
				udata->sesssionValid = true;
			else
				udata->sesssionValid = false;
		}
		udata->sesssionValid = isCookieValid(udata->expireTime);
		if (udata->sesssionValid && udata->alreadySessionSend)
			std::cout << "session is valid" << std::endl;
		else if (udata->alreadySessionSend)
			std::cout << "session is invalid" << std::endl;
	}
	else
		udata->sesssionValid = false;
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
	oss << "HTTP/1.1 " << response->returnState << " " << response->statusCodeMap[Utils::ftStoi(response->returnState)] << CRLF;
	oss << "Location: " << response->redirect << CRLF;
	oss << "Connection: close" << CRLF2;
	Utils::ftSend(response->clientFd, oss.str());
	return;
}

bool Worker::invalidResponse(ResponseData *response)
{
	if (!Utils::isFile(response->resourcePath))
	{
		if (Utils::needBody(response->method))

			return false;
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

// 문자열 A에서 문자열 B와 C 사이의 문자열 추출

// Response의 statusCode, contentType, charset, body 세팅
void Worker::setResponse(ResponseData *response, const std::string &resourceContent)
{
	// Status Content-Type charset 없는 경우에 대한 처리 추가에 대한 논의 필요
	if (Utils::extractSubstring(resourceContent, "Status: ", "\0") == "")
	{
		response->statusCode = 200;
		response->contentType = "text/html";
		response->charset = "utf-8";
		response->body = resourceContent;
	}
	else
	{
		response->statusCode = Utils::ftStoi(Utils::extractSubstring(resourceContent, "Status: ", " OK"));
		response->contentType = Utils::extractSubstring(resourceContent, "Content-Type: ", ";");
		response->charset = Utils::extractSubstring(resourceContent, "charset=", CRLF);
		response->body = Utils::extractSubstring(resourceContent, "\r\n\r\n", "\0");
	}
}
