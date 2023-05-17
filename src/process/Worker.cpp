/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/17 14:05:28 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/event.h>
#include <unistd.h>
#include "common_error.hpp"
#include "HTTPRequestParser.hpp"
#include "Worker.hpp"
#include "MimeTypesParser.hpp"
#include <sys/stat.h>

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

	if (request.method == "GET")
	{

		// std::string filepath = "/Users/han/42Seoul/webserv/cgi-bin/";
		// 파일 경로와 이름을 합칩니다.

		// std::ofstream outfile(fullpath);
		// Print response body
		// std::cout
		// << "Response body :\n\n\n\n"
		// << cgi.getResponseBody()
		// << std::endl;
		// CGI cgi("");
		// cgi.excuteCGI("./src/cgi-bin/hello3.py");
		//  getResponse(request, client_fd);
		if (isCGIRequest(request))
		{
			// 	std::string cgiPath = extractCGIPath(request);
			// std::string cgiPath = "/src/cgi-bin/upload.py";

			std::string filename = "helloworld.html";
			std::string filepath = "./assets/html/";
			std::string fullpath = filepath + filename;
			// 파일을 열고 문자열을 쓴 후 닫습니다.
			std::ofstream testCGI(fullpath);
			CGI cgi("upload.py");
			testCGI << cgi.excuteCGI("./src/cgi-bin/upload.py");
			testCGI << cgi.body_;

			// testCGI.close();

			std::cout
				<< "aaaaaaaaaaa   " << std::endl;
			// 	// 	cgi.excuteCGI(cgiPath);
			// 	// }
			// 	// else
			// 	// {
			// 	// 	getResponse(request, client_fd);
		}
		std::cout << "requestHandler port: " << request.port << ", Server[" << getSuitableServer(request.port) << "]" << std::endl;
		if (getSuitableServer(request.port) == -1)
			return;
		size_t nServer = static_cast<size_t>(getSuitableServer(request.port));
		ServerInfo thisServer = this->server.server[nServer];
		ResponseData *response = getResponseData(request, client_fd, thisServer);
		if (request.method == "GET" && (std::find(response->limit_except.begin(), response->limit_except.end(), "GET") != response->limit_except.end()))
		{
			getResponse(response);
		}
		if (request.method == "POST")
		{
		}
		else
		{
			// 현재는 location을 찾지못해 limit.except에서 판별이안되 넘어오는 경우도있음!
			// 잘못된 메서드일경우
			std::string response_body = "Method not allowed";
			std::string response_header = generateErrorHeader(405, response_body);
			write(response->clientFd, response_header.c_str(), response_header.length());
			write(response->clientFd, response_body.c_str(), response_body.length());
		}
		delete response;
	}
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
std::string Worker::getRootDirectory(const HTTPRequest &request, const ServerInfo &thisServer)
{
	//.ico파일일 경우 임의로 이미지폴더로 이동
	if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
		return "./assets/images";
	return thisServer.root;
}

bool Worker::isCGIRequest(const HTTPRequest &request)
{
	// 이 부분은 CGI 요청을 확인하는 로직을 구현합니다.
	// 예를 들어, 요청 URL에 특정 확장자(.cgi, .php 등)가 포함되어 있는지 확인할 수 있습니다.
	// 요청이 CGI 요청인 경우 true를 반환하고, 그렇지 않은 경우 false를 반환합니다.
	// return request.find(".py") != std::string::npos;
	size_t pos = request.path.find("cgi-bin");
	return (pos != std::string::npos);
	// (void)request;
	// return true;
}

/**
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param client_fd 웹 소켓
 */
void Worker::getResponse(ResponseData *response)
{
	struct stat st;
	const char *path = response->resourcePath.c_str();
	if (!stat(path, &st))
		std::cerr << "Failed to get information about " << path << std::endl;
	// 리소스를 찾지 못했다면 404페이지로 이동
	if (!S_ISREG(st.st_mode))
	{
		if (!response->index.empty())
		{
			response->resourcePath = response->root + '/' + response->index;
			std::ifstream resource_file(response->resourcePath);
			if (!resource_file.is_open())
			{
				return errorResponse(response->clientFd);
			}
		}
		else
			return errorResponse(response->clientFd);
	}
	std::ifstream resource_file(response->resourcePath);
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
	resource_file.close();
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
/**
 * path중 location에 매칭되는게있는지 판단하고, 매칭되는게 몇번째 location인지 찾는다.
 *
 * @param request request 를 파싱완료한 구조체
 * @param thisServer 현재 해당하는 서버
 * @param idx 몇번째 location블록과 매칭되는지 값을 받아온다.
 * @return 매칭된다면 true 그렇지않다면 false
 */
bool matchLocation(const HTTPRequest &request, ServerInfo &thisServer, size_t &idx)
{
	for (size_t i = 0; i < thisServer.location.size(); ++i)
	{
		thisServer.location[i].value.erase(thisServer.location[i].value.find_last_not_of(' ') + 1);
		if (thisServer.location[i].value == request.path)
		{
			idx = i;
			return (true);
		}
	}
	size_t pos = request.path.rfind('/');
	while (pos != std::string::npos)
	{
		std::string tmp = request.path.substr(0, pos); //
		for (size_t i = 0; i < thisServer.location.size(); ++i)
		{
			if (thisServer.location[i].value == tmp)
			{
				idx = i;
				return (true);
			}
		}
		tmp = tmp.erase(pos);
		pos = tmp.rfind('/');
	}
	return (false);
}

/**
 * ResponseDate구조체를 얻어옴. 만약 location과 일치한다면 location을 우선으로 가져옴
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 웹 소켓
 * @param thisServer 현재 해당하는 서버
 * @return 전부 채워진 ResponseDate구조체
 */
ResponseData *Worker::getResponseData(const HTTPRequest &request, const int &client_fd, ServerInfo &thisServer)
{
	ResponseData *response = new ResponseData;
	response->index = thisServer.index;
	response->clientFd = client_fd;
	response->root = getRootDirectory(request, thisServer);
	size_t i = 0;
	if (matchLocation(request, thisServer, i))
	{
		for (size_t j = 0; j < thisServer.location[i].block.size(); ++j)
		{
			if (thisServer.location[i].block[j].name == "root")
				response->root = thisServer.location[i].block[j].value;
			else if (thisServer.location[i].block[j].name == "index")
				response->index = thisServer.location[i].block[j].value;
			// else if (thisServer.location[i].block[j].name == "autoindex") //켜져있다면 동적으로 index를 만들어야함
			// 	thisServer.location[i].block[j].value == "on" ? :
			else if (thisServer.location[i].block[j].name == "limit_except")
			{
				size_t pos = thisServer.location[i].block[j].value.find(' ');
				size_t start = 0;
				while (pos != std::string::npos)
				{
					std::string tmp = thisServer.location[i].block[j].value.substr(start, pos - start);
					response->limit_except.push_back(tmp);
					start = pos;
					while (thisServer.location[i].block[j].value[start] != '\0' && thisServer.location[i].block[j].value[start] == ' ')
						start++;
					pos = thisServer.location[i].block[j].value.find(' ', start);
				}
				std::string tmp = thisServer.location[i].block[j].value.substr(start);
				response->limit_except.push_back(tmp);
			}
			else if (thisServer.location[i].block[j].name == "return")
			{
				size_t pos = thisServer.location[i].block[j].value.find(' ');
				size_t start = 0;
				std::string tmp = thisServer.location[i].block[j].value.substr(start, pos - start);
				response->return_state = tmp;
				start = pos;
				while (thisServer.location[i].block[j].value[start] != '\0' && thisServer.location[i].block[j].value[start] == ' ')
					start++;
				tmp = thisServer.location[i].block[j].value.substr(start);
				response->redirect = tmp;
			}
		}
	}
	if (response->limit_except.size() == 0)
		response->limit_except = thisServer.limitExcept;
	response->resourcePath = response->root + request.path;
	return (response);
}

// //broad 페이지 작업중입니다...
// void recursionDir(const std::string &path, std::stringstream &broadHtml, DIR *dirPtr)
// {
//     dirent *file;
//     broadHtml << "<p>";
//     if ((file = readdir(dirPtr)) == NULL)
//         return;
//     broadHtml << "<a href=" << path << "/" << file->d_name << ">" << file->d_name << "</a><p>";
//     recursionDir(path, broadHtml, dirPtr);
//     return;
// }

// void broad(const HTTPRequest &request, int client_fd, Config &config)
// {
//     std::stringstream broadHtml;
//     broadHtml << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>broad page</title></head><body><h1>show</h1>";
//     DIR *dirPtr = NULL;
//     std::string path = "/example"; // location + path로 교체예정
//     if ((dirPtr = opendir(path.c_str())) != NULL)
//     {
//         std::cout << "broad: location path err" << std::endl;
//         return;
//     }
//     recursionDir(path, broadHtml, dirPtr);
//     broadHtml << "</body></html>"
//     std::string tmp = broadHtml.str();
//     /* 헤더를 작성해주는과정 */
//     MimeTypesParser mime(config);
//     std::string contentType = mime.getMimeType("html");
//     std::string response_header = generateHeader(tmp, contentType);
//     write(client_fd, response_header.c_str(), response_header.length());
//     write(client_fd, tmp.c_str(), tmp.length()); //완성된 html 을 body로 보냄
// }
