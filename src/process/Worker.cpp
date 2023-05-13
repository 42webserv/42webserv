/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 16:43:20 by seokchoi         ###   ########.fr       */
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
		delete (sockets[i]);
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
				std::vector<int>::iterator found;

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
					found = std::find(sockets[k]->clientFds.begin(), sockets[k]->clientFds.end(), fd);
					if (found == sockets[k]->clientFds.end())
						continue;

					// 돌아가는 방식 보고 싶을때

					// std::cout << fd << " 의 Read event  소켓 포트 번호는 " << sockets[k]->_port << std::endl;
					// std::cout << "그리고 socket에 저장되어있는 fd들은 [";
					// for (size_t i = 0; i < sockets[k]->clientFds.size(); i++)
					// 	std::cout << sockets[k]->clientFds[i] << ", ";
					// std::cout << "]" << std::endl;

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
						}
						if (n < 1)
						{
							// if (n < 0) // 여기 들어온다는 것은 읽지 못하는 것을 읽었다는 뜻인데 그럼...
							// 	std::cerr << "Client read error!" << '\n';
							std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;

							struct kevent new_event;
							EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
							event_list.push_back(new_event);
						}
					}
				}
				else if (event.filter == EVFILT_WRITE)
				{
					found = std::find(sockets[k]->clientFds.begin(), sockets[k]->clientFds.end(), fd);
					if (found == sockets[k]->clientFds.end())
						continue;

					// 돌아가는 방식 보고 싶을때

					// std::cout << fd << " 의 Write event  소켓 포트 번호는 " << sockets[k]->_port << std::endl;
					// std::cout << "그리고 socket에 저장되어있는 fd들은 [";
					// for (size_t i = 0; i < sockets[k]->clientFds.size(); i++)
					// 	std::cout << sockets[k]->clientFds[i] << ", ";
					// std::cout << "]" << std::endl;

					result = parser.parse(clients[fd]);
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

	if (request.method == "GET")
	{
		if (isCGIRequest(request))
		{
			CGI cgi("hello.py");
			std::string cgiPath = extractCGIPath(request);
			std::cout << "aaaaaaaaaaa   " << cgiPath << std::endl;
			cgi.excuteCGI(cgiPath);
		}
		else
		{
			getResponse(request, client_fd);
		}
	}
	if (request.method == "POST")
	{
	}
	else
	{
		// 잘못된 메서드일경우
		std::string response_body = "Method not allowed";
		std::string response_header = generateErrorHeader(405, response_body);
		write(client_fd, response_header.c_str(), response_header.length());
		write(client_fd, response_body.c_str(), response_body.length());
	}
}

bool Worker::isCGIRequest(const HTTPRequest &request)
{
	// 이 부분은 CGI 요청을 확인하는 로직을 구현합니다.
	// 예를 들어, 요청 URL에 특정 확장자(.cgi, .php 등)가 포함되어 있는지 확인할 수 있습니다.
	// 요청이 CGI 요청인 경우 true를 반환하고, 그렇지 않은 경우 false를 반환합니다.
	// return request.find(".py") != std::string::npos;
	(void)request;
	return true;
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param client_fd 웹 소켓
 */
void Worker::getResponse(const HTTPRequest &request, int client_fd)
{
	// root_dir에 관한내용은 conf에서 가져옴
	std::string root_dir = "./assets/html"; // Root directory for serving static files
	//.ico파일일 경우 임의로 이미지폴더로 이동
	if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
		root_dir = "./assets/images";
	std::string resource_path = root_dir + (request.path == "/" ? "/index.html" : request.path);
	std::ifstream resource_file(resource_path);
	// 리소스를 찾지 못했다면 404페이지로 이동
	if (!resource_file.good())
		return errorResponse(client_fd);

	// 경로에서 확장자 찾아준 뒤, Content-Type 찾기
	std::vector<std::string> tokens;
	std::istringstream iss(resource_path);
	std::string token;
	while (std::getline(iss, token, '.'))
		tokens.push_back(token);
	std::string extension = tokens.back();
	MimeTypesParser mime(this->config);
	std::string contentType = mime.getMimeType(extension);

	std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
								 std::istreambuf_iterator<char>());
	std::string response_header = generateHeader(resource_content, contentType);
	write(client_fd, response_header.c_str(), response_header.length());
	write(client_fd, resource_content.c_str(), resource_content.length());
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

// CGI 처리

std::string Worker::extractCGIPath(const HTTPRequest &request)
{
	// CGI 경로 추출 로직을 구현합니다.
	// 예를 들어, 요청 URL에서 경로 부분을 추출하는 방식으로 구현할 수 있습니다.
	// 추출된 경로를 반환합니다.
	// 예: http://example.com/cgi-bin/cgi_program.cgi
	// 추출된 경로: /cgi-bin/cgi_program.cgi
	std::string cgiPath = request.path.substr(request.path.find("/cgi-bin/"));
	return cgiPath;
}
