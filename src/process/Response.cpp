/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:43 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/18 19:31:04 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
 * A default constructor
 */

// Response::Response() {}

Response::Response(int port, Server &server)
{
    size_t nServer = static_cast<size_t>(getSuitableServer(port, server));
    this->thisServer = server.server[nServer];
}

/*
 * A destructor
 */
Response::~Response()
{
}

/**
 * 특정 포트번호가 몇 번째 서버에 위치하는지 찾아서 위치값 반환. 서버 내에서 포트번호를 찾지 못할경우 -1 반환
 *
 * @param port 위치 찾고싶은 포트번호
 * @return 서버 위치
 */
int Response::getSuitableServer(int port, Server &server)
{
    std::vector<ServerInfo> serv = server.server;
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
 * ResponseDate구조체를 얻어옴. 만약 location과 일치한다면 location을 우선으로 가져옴
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 웹 소켓
 * @param thisServer 현재 해당하는 서버
 * @return 전부 채워진 ResponseDate구조체
 */
ResponseData *Response::getResponseData(const HTTPRequest &request, const int &client_fd)
{
    ResponseData *response = new ResponseData;
    response->index = this->thisServer.index;
    response->clientFd = client_fd;
    response->root = getRootDirectory(request, thisServer);
    int idx = matchLocation(request, thisServer);
    if (idx != -1)
    {
        size_t i = static_cast<size_t>(idx);
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

/**
 * 해당 서버의 root 경로를 반환. 요청이 favicon.ico 일 경우, 미리 지정한 이미지 경로 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param thisServer 현재 해당하는 서버
 * @return 경로 문자열
 */
std::string Response::getRootDirectory(const HTTPRequest &request, const ServerInfo &thisServer)
{
    //.ico파일일 경우 임의로 이미지폴더로 이동
    if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
        return "./assets/images";
    return thisServer.root;
}

/*
 * @param request request 를 파싱완료한 구조체
 * @param thisServer 현재 해당하는 서버
 * @param idx 몇번째 location블록과 매칭되는지 값을 받아온다.
 * @return 매칭된다면 true 그렇지않다면 false
 */
int Response::matchLocation(const HTTPRequest &request, ServerInfo &thisServer)
{
    for (size_t i = 0; i < thisServer.location.size(); ++i)
    {
        thisServer.location[i].value.erase(thisServer.location[i].value.find_last_not_of(' ') + 1);
        if (thisServer.location[i].value == request.path)
            return static_cast<int>(i);
    }
    return -1;
}
