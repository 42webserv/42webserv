/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:43 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/30 13:39:15 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
#include "commonProcess.hpp"
#include "Response.hpp"

/*
 * A default constructor
 */

Response::Response() {}

/*
 * A destructor
 */
Response::~Response() {}

/**
 * 특정 포트번호가 몇 번째 서버에 위치하는지 찾아서 위치값 반환. 서버 내에서 포트번호를 찾지 못할경우 -1 반환
 *
 * @param port 위치 찾고싶은 포트번호
 * @return 서버 위치
 */
int Response::getSuitableServer(int port, Server &serverManager)
{
    std::vector<ServerInfo> &servers = serverManager.servers;

    for (std::vector<ServerInfo>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        ServerInfo info = *it;
        for (size_t i = 0; i < info.ports.size(); i++)
        {
            if (info.ports[i] == port)
                return static_cast<int>(it - servers.begin());
        }
    }
    return -1;
}

/**
 * ResponseDate구조체를 얻어옴. 만약 location과 일치한다면 location을 우선으로 가져옴
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 웹 소켓
 * @param config conf 파일을 파싱한 클래스
 * @param serverManger 서버 관리 클래스
 * @return 전부 채워진 ResponseDate구조체
 */
ResponseData *Response::getResponseData(const HTTPRequest &request, const int &client_fd, Config &config, Server &serverManger)
{
    int index = getSuitableServer(request.port, serverManger);
    if (index < 0)
        index = 0;
    ServerInfo server = serverManger.servers[index];
    ResponseData *response = new ResponseData;
    response->server = server;
    response->index = server.index;
    response->path = request.path;
    response->method = request.method;
    response->clientFd = client_fd;
    response->root = getRootDirectory(request, server);
    response->location = findLocation(request, server.locations);
    if (response->location != NULL)
    {
        setUpRoot(response);
        setUpIndex(response);
        setUpAutoindex(response);
        setUpLimitExcept(response);
        setUpReturnState(response);
    }
    if (response->limitExcept.size() < 1)
        response->limitExcept = server.limitExcepts;
    response->resourcePath = response->root + "/" + response->index;
    // 경로에서 확장자 찾아준 뒤, Content-Type 찾기
    std::vector<std::string> tokens;
    std::istringstream iss(response->resourcePath);
    std::string token;
    while (std::getline(iss, token, '.'))
        tokens.push_back(token);
    std::string extension = tokens.back();
    MimeTypesParser mime(config);
    response->contentType = mime.getMimeType(extension);
    response->body = request.body;
    response->contentLength = response->body.length();
    return (response);
}

/**
 * location 블록 내부에서 root 지시자를 찾아 responseData->root 세팅
 *
 * @param response 반환될 responseData 구조체
 */
void Response::setUpRoot(ResponseData *response)
{
    std::vector<Directive> &block = response->location->block;
    for (size_t i = 0; i < block.size(); i++)
    {
        Directive &dir = block[i];
        if (dir.name == "root")
        {
            response->root = dir.value;
            break;
        }
    }
}

/**
 * location 블록 내부에서 index 지시자를 찾아 responseData->index 세팅
 *
 * @param response 반환될 responseData 구조체
 */
void Response::setUpIndex(ResponseData *response)
{
    std::vector<Directive> &block = response->location->block;

    for (size_t i = 0; i < block.size(); i++)
    {
        Directive &dir = block[i];
        if (dir.name == "index")
        {
            response->index = dir.value;
            break;
        }
    }
}

/**
 * location 블록 내부에서 autoindex 지시자를 찾아 responseData->autoindex 세팅
 *
 * @param response 반환될 responseData 구조체
 */
void Response::setUpAutoindex(ResponseData *response)
{
    std::vector<Directive> &block = response->location->block;

    response->autoindex = false;
    for (size_t i = 0; i < block.size(); i++)
    {
        Directive &dir = block[i];
        if (dir.name == "autoindex")
        {
            response->autoindex = (dir.value == "on") ? true : false;
            break;
        }
    }
}

/**
 * location 블록 내부에서 limit_except 지시자를 찾아 responseData->limitExcept 세팅
 *
 * @param response 반환될 responseData 구조체
 */
void Response::setUpLimitExcept(ResponseData *response)
{
    std::vector<Directive> &block = response->location->block;

    for (size_t i = 0; i < block.size(); i++)
    {
        Directive &dir = block[i];
        if (dir.name == "limit_except")
        {
            size_t pos = dir.value.find(' ');
            size_t start = 0;
            while (pos != std::string::npos)
            {
                std::string tmp = dir.value.substr(start, pos - start);
                response->limitExcept.push_back(tmp);
                start = pos;
                while (dir.value[start] != '\0' && dir.value[start] == ' ')
                    start++;
                pos = dir.value.find(' ', start);
            }
            std::string tmp = dir.value.substr(start);
            response->limitExcept.push_back(tmp);
            break;
        }
    }
}

/**
 * location 블록 내부에서 return 지시자를 찾아 responseData->returnState, redirect 세팅
 *
 * @param response 반환될 responseData 구조체
 */
void Response::setUpReturnState(ResponseData *response)
{
    std::vector<Directive> &block = response->location->block;

    for (size_t i = 0; i < block.size(); i++)
    {
        Directive &dir = block[i];
        if (dir.name == "return")
        {
            size_t pos = dir.value.find(' ');
            size_t start = 0;
            std::string tmp = dir.value.substr(start, pos - start);
            response->returnState = tmp;
            start = pos;
            while (dir.value[start] != '\0' && dir.value[start] == ' ')
                start++;
            tmp = dir.value.substr(start);
            response->redirect = tmp;
            break;
        }
    }
}

/**
 * 해당 서버의 root 경로를 반환. 요청이 favicon.ico 일 경우, 미리 지정한 이미지 경로 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param thisServer 현재 해당하는 서버
 * @return 경로 문자열
 */
std::string Response::getRootDirectory(const HTTPRequest &request, const ServerInfo &server)
{
    //.ico파일일 경우 임의로 이미지폴더로 이동
    if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ICO_EXTENSION)
        return "./assets/images"; // TODO image 경로 지정하는 방법 찾아보기
    return server.root;
}

std::string Response::delQuery(std::string path)
{
    size_t pos = path.rfind('/');
    size_t pos_q = path.rfind('?');
    if (pos_q > pos)
        return (path.substr(0, pos_q));
    return (path);
}

/**
 * request의 path와 매칭되는 location 블록을 찾아 반환
 * @param request request를 파싱완료한 구조체
 * @param locations 현재 해당하는 서버의 location 목록
 * @return 매칭된다면 location 지시문, 그렇지않다면 NULL
 */
Directive *Response::findLocation(const HTTPRequest &request, std::vector<Directive> &locations)
{
    for (std::vector<Directive>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        Directive &location = *it;
        location.value.erase(location.value.find_last_not_of(' ') + 1);
        if (location.value == delQuery(request.path))
            return &location;
        if (location.value != "/" && request.method == PUT) //PUT에서 작동하는데 먼지모르겠음
        {
            size_t pos = request.path.find(location.value);
            if (pos != std::string::npos)
                return &location;
        }
    }
    size_t pos = request.path.rfind('/'); // 처음엔 확장자만 지워서 매칭되는 location을 찾음
    while (pos != std::string::npos)
    {
        std::string tmp = delQuery(request.path.substr(0, pos));
        for (std::vector<Directive>::iterator it = locations.begin(); it != locations.end(); it++)
        {
            Directive &location = *it;
            location.value.erase(location.value.find_last_not_of(' ') + 1);
            if (location.value == tmp)
            {
                std::string file = delQuery(request.path.substr(pos, request.path.length()));
                std::cout << "location : " << tmp << std::endl;
                std::string root;
                for (size_t i = 0; i < location.block.size(); i++)
                {
                    if (location.block[i].name == "root")
                    {
                        std::cout << "root : " << location.block[i].value << std::endl;
                        root = location.block[i].value;
                        if (isDirectory(root + file))
                        {
                            location.block[i].value = root + file;
                            return &location;
                        }
                        else
                        {
                            for (size_t j = 0; j < location.block.size(); j++)
                            {
                                if (location.block[j].name == "index")
                                {
                                    location.block[j].value = file;
                                    return &location;
                                }
                            }
                            Directive index;
                            index.name = "index";
                            index.value = file;
                            location.block.push_back(index);
                            return &location;
                        }
                    }
                }
                return &location;
            }
        }
        tmp = tmp.erase(pos);
        pos = tmp.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
    }
    return NULL;
}
