/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/17 17:24:47 by yje              ###   ########.fr       */
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
#include <dirent.h>

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

//  * 각각 method 실행과 해당 포트에 response를 보내줌
//  *
//  * @param request request 를 파싱완료한 구조체
//  * @param client_fd 서버의 fd

void Worker::requestHandler(const HTTPRequest &request, int client_fd)
{
	if (request.method == "GET")
	{
		if (isCGIRequest(request))
		{
			CGI cgi("");
			std::string cgiFullPath = "./src" + request.path + ".py";
			std::string filename = "result.html";
			std::string filepath = "./assets/html/";
			std::string fullpath = filepath + filename;
			// 파일을 열고 문자열을 쓴 후 닫습니다.
			std::ofstream testCGI(fullpath);
			std::cout << "cgipath -> full :  " << cgiFullPath << std::endl;
			testCGI << cgi.excuteCGI(cgiFullPath);
			testCGI.close();
		}
	}
	std::cout << "requestHandler port: " << request.port << ", Server[" << getSuitableServer(request.port) << "]" << std::endl;
	if (getSuitableServer(request.port) == -1)
		return;
	size_t nServer = static_cast<size_t>(getSuitableServer(request.port));
	ServerInfo thisServer = this->server.server[nServer];
	ResponseData *response = getResponseData(request, client_fd, thisServer);
	//현재 메서드와 limit을 비교후 바로 404 갈지 실행한지 분기
	if (request.method == "GET" && (std::find(response->limit_except.begin(), response->limit_except.end(), "GET") != response->limit_except.end()))
	// if (request.method == "GET")
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
	if (!stat(response->resourcePath.c_str(), &st)) //파일인지 디렉토리인지 검사하기위해 stat함수 사용
		std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
	if (!S_ISREG(st.st_mode)) //root + index을 검사해 파일이 아닐시 if로 분기
	{
		response->resourcePath = response->root + response->cgi->path; //root + path로 다시 검사
		// std::cout << response->resourcePath
		std::memset(&st, 0, sizeof(st));
		if (!stat(response->resourcePath.c_str(), &st))
			std::cerr << "Failed to get information about " << response->resourcePath.c_str() << std::endl;
		if (!S_ISREG(st.st_mode))
		{
			if (response->autoindex)
			{
				broad(response);
				return ;
			}
			else
				return errorResponse(response->clientFd);
		}
	}
	std::ifstream resource_file(response->resourcePath); //위에서 stat함수로 파일검사는 완료
	if (!resource_file.is_open()) //혹시 open이 안될수있으니 한번더 체크
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
	// while돌면서 "/" 부분을 지우고 찾는 부분인데 "/" 까지 지우지 때문에 "/" 하나와 매칭되지않음.
	size_t pos = request.path.rfind('.'); // 처음엔 확장자만 지워서 매칭되는 location을 찾음
	while (pos != std::string::npos)
	{
		std::string tmp = request.path.substr(0, pos);
		for (size_t i = 0; i < thisServer.location.size(); ++i)
		{
			if (thisServer.location[i].value == tmp)
			{
				idx = i;
				return (true);
			}
		}
		tmp = tmp.erase(pos);
		pos = tmp.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
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
	{
		Cgi *cgi = new Cgi;
		cgi->addr = request.addr;
		cgi->body = request.body;
		cgi->port = request.port;
		cgi->name = thisServer.serverName;
		cgi->query = request.query;
		cgi->path = request.path;
		response->cgi = cgi;
	}
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
			else if (thisServer.location[i].block[j].name == "autoindex")
			{
				thisServer.location[i].block[j].value == "on" ? response->autoindex = true : response->autoindex = false;
			}
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
	response->resourcePath = response->root + "/" + response->index; //index 정보는 server 또는 location에서 가져왔음
	return (response);
}

void Worker::broad(ResponseData *response)
{
    std::stringstream broadHtml;
    broadHtml << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>broad page</title></head><body><h1>show</h1>";
    DIR *dirPtr = NULL;
	dirent *file;
	std::cout << "------ " << response->root << std::endl;
    // if ((dirPtr = opendir(response->root.c_str())) != NULL)
    if ((dirPtr = opendir(response->root.c_str())) == NULL)
    {
        std::cout << "broad: location path err" << std::endl;
        return;
    }
	while ((file = readdir(dirPtr)))
	{
		broadHtml << "<p><a href=" << response->root << "/" << file->d_name << ">" << file->d_name << "</a></p>";
		// broadHtml << "<p><a href=" << "/index" << "/" << file->d_name << ">" << file->d_name << "</a></p>";
	}
    broadHtml << "</body></html>";
    std::string tmp = broadHtml.str();
    /* 헤더를 작성해주는과정 */
    MimeTypesParser mime(config);
    std::string contentType = mime.getMimeType("html");
    std::string response_header = generateHeader(tmp, contentType);
    write(response->clientFd, response_header.c_str(), response_header.length());
    write(response->clientFd, tmp.c_str(), tmp.length()); //완성된 html 을 body로 보냄
}
