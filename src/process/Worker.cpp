/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 13:24:32 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/event.h>
#include <unistd.h>
#include "common_error.hpp"
#include "HTTPRequestParser.hpp"
#include "Worker.hpp"
#include "MimeTypesParser.hpp"

Worker::Worker(Master &master) : kq(master.kq), signal(master.getEvents()), event_list(master.getEvents()), config(master.getConfig()), server(master.getServer())
{
	// Create sockets
	for (size_t i = 0; i < server.server.size(); i++)
	{
		for (size_t j = 0; j < server.server[i].port.size(); j++)
		{
			sockets.push_back(new Socket(master.getEvents(), server.server[i].port[j]));
		}
	}
}

Worker::~Worker()
{
	for (size_t i = 0; i < sockets.size(); i++)
	{
		delete sockets[i];
	}
}

void Worker::run()
{
	struct kevent events[10];
	struct kevent event;
	int nevents;
	int fd;
	std::map<int, std::string> clients;
	HTTPRequestParser parser;
	HTTPRequest *result = NULL;

	while (true)
	{
		// event_list	-> events we want to monitor
		// event		-> events that were triggered
		nevents = kevent(kq, &event_list[0], event_list.size(), events, 10, NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		event_list.clear();

		for (size_t k = 0; k < sockets.size(); k++)
		{
			for (int i = 0; i < nevents; i++)
			{
				event = events[i];
				fd = event.ident;

				if (event.flags & EV_ERROR)
				{
					// handle error
					if (fd == sockets[k]->server_fd)
						// 서버 소켓 에러
						error_exit("Server socket error");
					else
					{
						// 클라이언트 소켓 에러 아니면 다른 에러
						if (clients.find(fd) != clients.end())
							sockets[k]->disconnectClient(fd, clients);
					}
				}
				if (event.filter == EVFILT_READ)
				{
					if (fd == sockets[k]->server_fd)
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
							std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;
						}
						result = parser.parse(clients[fd]);
						if (sockets[k]->_port != parser.getPort(*result))
							continue;
						std::cout << "hehehehehehehere" << std::endl;
						if (n < 1)
						{
							if (n < 0)
								std::cerr << "Client read error!" << '\n';
							struct kevent new_event;
							EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
							event_list.push_back(new_event);
						}
					}
				}
				else if (event.filter == EVFILT_WRITE)
				{
					if (!result)
						result = parser.parse(clients[fd]);
					if (sockets[k]->_port != parser.getPort(*result))
						continue;
					if (clients.find(fd) != clients.end())
					{
						if (result)
						{
							// TODO: HTTP Response 구현
							this->requestHandler(*result, fd);
							delete result;
						}
						else
							std::cout << "Failed to parse request" << std::endl;
						sockets[k]->disconnectClient(fd, clients);
						clients[fd].clear();
					}
					// 큰 파일 처리할 때
					// off_t offset = (off_t)event.udata;
					// off_t len = 0;
					// if (sendfile(junk, fd, offset, &len, NULL, 0) != 0)
					// {
					// 	if (errno == EAGAIN)
					// 	{
					// 		EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, (void *)(offset + len));
					// 		event_list.push_back(event);
					// 	}
					// }
					// bytes_written += len;
				}
				else if (event.filter == EVFILT_SIGNAL)
					signal.handleEvent(event, sockets);
				// system("leaks webserv");
			}
		}
	}
}

/**
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 서버의 fd
 */
void Worker::requestHandler(const HTTPRequest &request, int client_fd)
{
	std::cout << "requestHandler port: " << request.port << ", Server[" << getSuitableServer(request.port) << "]" << std::endl;
	if (getSuitableServer(request.port) == -1)
		return;
	size_t nServer = static_cast<size_t>(getSuitableServer(request.port));
	ServerInfo thisServer = this->server.server[nServer];
	ResponseData *response = new ResponseData;
	response->index = thisServer.index;
	response->clientFd = client_fd;
	response->root = getRootDirectory(request, thisServer);
	response->resourcePath = response->root + (request.path == "/" ? "/" + thisServer.index : request.path);
	if (request.method == "GET")
	{
		getResponse(response);
	}
	else
	{
		// 잘못된 메서드일경우
		std::string response_body = "Method not allowed";
		std::string response_header = generateErrorHeader(405, response_body);
		write(response->clientFd, response_header.c_str(), response_header.length());
		write(response->clientFd, response_body.c_str(), response_body.length());
	}
	delete response;
}

/**
 * 특정 포트번호가 몇 번째 서버에 위치하는지 찾아서 위치값 반환. 서버 내에서 포트번호를 찾지 못할경우 -1 반환
 *
 * @param port 위치 찾고싶은 포트번호
 * @return 서버 위치
 */
int Worker::getSuitableServer(int port)
{
	std::vector<ServerInfo> serv = this->server.server;
	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t j = 0; j < serv[i].port.size(); j++)
		{
			if (serv[i].port[j] == port)
				return static_cast<int>(i);
		}
	}
	return -1;
}

/**
 * 해당 서버의 root 경로를 반환. 요청이 favicon.ico 일 경우, 미리 지정한 이미지 경로 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param thisServer 현재 해당하는 서버
 * @return 경로 문자열
 */
std::string Worker::getRootDirectory(const HTTPRequest &request, ServerInfo &thisServer)
{
	//.ico파일일 경우 임의로 이미지폴더로 이동
	if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
		return "./assets/images";
	return thisServer.root;
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param client_fd 웹 소켓
 */
void Worker::getResponse(ResponseData *response)
{
	std::ifstream resource_file(response->resourcePath);
	// 리소스를 찾지 못했다면 404페이지로 이동
	if (!resource_file.good())
		return errorResponse(response->clientFd);

	// 경로에서 확장자 찾아준 뒤, Content-Type 찾기
	std::vector<std::string> tokens;
	std::istringstream iss(response->resourcePath);
	std::string token;
	while (std::getline(iss, token, '.'))
		tokens.push_back(token);
	std::string extension = tokens.back();
	MimeTypesParser mime(this->config);
	std::string contentType = mime.getMimeType(extension);

	std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
								 std::istreambuf_iterator<char>());
	std::string response_header = generateHeader(resource_content, contentType);
	write(response->clientFd, response_header.c_str(), response_header.length());
	write(response->clientFd, resource_content.c_str(), resource_content.length());
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
	std::string error_content((std::istreambuf_iterator<char>(error_file)),
							  std::istreambuf_iterator<char>());
	std::string error_header = generateErrorHeader(404, error_content);
	write(client_fd, error_header.c_str(), error_header.length());
	write(client_fd, error_content.c_str(), error_content.length());
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @param contentType Content-Type
 * @return 최종완성된 헤더를 반환함
 */
std::string Worker::generateHeader(const std::string &content, const std::string &contentType)
{
	HTTPRequestParser parser;
	std::ostringstream oss;

	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "Content-Type: " << contentType << "\r\n"; // MIME type can be changed as needed
	oss << "Connection: close\r\n\r\n";
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
	// oss << "HTTP/1.1 " << status_code << " " << message << "\r\n";
	oss << "HTTP/1.1 " << status_code << " OK\r\n";
	oss << "Content-Length: " << message.length() << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Connection: close\r\n\r\n";
	return oss.str();
}
