/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:43 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/29 16:52:29 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
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
        setUpRoot(response->location->block, response);
        setUpIndex(response->location->block, response);
        setUpAutoindex(response->location->block, response);
        setUpLimitExcept(response->location->block, response);
        setUpReturnState(response->location->block, response);
    }
    if (response->limitExcept.size() == 0)
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
 * @param locationBlock 파싱된 location 블록
 * @param response 반환될 responseData 구조체
 */
void Response::setUpRoot(std::vector<Directive> &locationBlock, ResponseData *response)
{
    for (size_t i = 0; i < locationBlock.size(); i++)
    {
        if (locationBlock[i].name == "root")
        {
            response->root = locationBlock[i].value;
            break;
        }
    }
}

/**
 * location 블록 내부에서 index 지시자를 찾아 responseData->index 세팅
 *
 * @param locationBlock 파싱된 location 블록
 * @param response 반환될 responseData 구조체
 */
void Response::setUpIndex(std::vector<Directive> &locationBlock, ResponseData *response)
{
    for (size_t i = 0; i < locationBlock.size(); i++)
    {
        if (locationBlock[i].name == "index")
        {
            response->index = locationBlock[i].value;
            break;
        }
    }
}

/**
 * location 블록 내부에서 autoindex 지시자를 찾아 responseData->autoindex 세팅
 *
 * @param locationBlock 파싱된 location 블록
 * @param response 반환될 responseData 구조체
 */
void Response::setUpAutoindex(std::vector<Directive> &locationBlock, ResponseData *response)
{
    response->autoindex = false;
    for (size_t i = 0; i < locationBlock.size(); i++)
    {
        if (locationBlock[i].name == "autoindex")
        {
            locationBlock[i].value == "on" ? response->autoindex = true : response->autoindex = false;
            break;
        }
    }
}

/**
 * location 블록 내부에서 limit_except 지시자를 찾아 responseData->limitExcept 세팅
 *
 * @param locationBlock 파싱된 location 블록
 * @param response 반환될 responseData 구조체
 */
void Response::setUpLimitExcept(std::vector<Directive> &locationBlock, ResponseData *response)
{
    for (size_t i = 0; i < locationBlock.size(); i++)
    {
        if (locationBlock[i].name == "limit_except")
        {
            size_t pos = locationBlock[i].value.find(' ');
            size_t start = 0;
            while (pos != std::string::npos)
            {
                std::string tmp = locationBlock[i].value.substr(start, pos - start);
                response->limitExcept.push_back(tmp);
                start = pos;
                while (locationBlock[i].value[start] != '\0' && locationBlock[i].value[start] == ' ')
                    start++;
                pos = locationBlock[i].value.find(' ', start);
            }
            std::string tmp = locationBlock[i].value.substr(start);
            response->limitExcept.push_back(tmp);
            break;
        }
    }
}

/**
 * location 블록 내부에서 return 지시자를 찾아 responseData->returnState, redirect 세팅
 *
 * @param locationBlock 파싱된 location 블록
 * @param response 반환될 responseData 구조체
 */
void Response::setUpReturnState(std::vector<Directive> &locationBlock, ResponseData *response)
{
    for (size_t i = 0; i < locationBlock.size(); i++)
    {
        if (locationBlock[i].name == "return")
        {
            size_t pos = locationBlock[i].value.find(' ');
            size_t start = 0;
            std::string tmp = locationBlock[i].value.substr(start, pos - start);
            response->returnState = tmp;
            start = pos;
            while (locationBlock[i].value[start] != '\0' && locationBlock[i].value[start] == ' ')
                start++;
            tmp = locationBlock[i].value.substr(start);
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

// int Response::matchLocation(const HTTPRequest &request, ServerInfo &server)
// {
//     for (size_t i = 0; i < server.locations.size(); ++i)
//     {
//         Directive location = server.locations[i];
//         location.value.erase(location.value.find_last_not_of(' ') + 1);
//         if (location.value == request.path)
//             return static_cast<int>(i);
//         if (location.value != "/")
//         {
//             size_t pos = request.path.find(location.value);
//             if (pos != std::string::npos)
//                 return static_cast<int>(i);
//         }
//     }
//     size_t pos = request.path.rfind('/'); // 처음엔 확장자만 지워서 매칭되는 location을 찾음
//     while (pos != std::string::npos)
//     {
//         std::string tmp = request.path.substr(0, pos);
//         for (size_t i = 0; i < thisServer.location.size(); ++i)
//         {
//             if (thisServer.location[i].value == tmp)
//                 return static_cast<int>(i);
//         }
//         tmp = tmp.erase(pos);
//         pos = tmp.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
//     }
//     return -1;
// }

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
        if (location.value == request.path)
            return &location;
        if (location.value != "/" && request.method == "PUT")
        {
            size_t pos = request.path.find(location.value);
            if (pos != std::string::npos)
                return &location;
        }
    }
    size_t pos = request.path.rfind('/'); // 처음엔 확장자만 지워서 매칭되는 location을 찾음
    while (pos != std::string::npos)
    {
        std::string tmp = request.path.substr(0, pos);
        for (std::vector<Directive>::iterator it = locations.begin(); it != locations.end(); it++)
        {
            Directive &location = *it;
            location.value.erase(location.value.find_last_not_of(' ') + 1);
            if (location.value == tmp)
            {
                std::string file = request.path.substr(pos, request.path.length());
                std::cout << "location : " << tmp << std::endl;
                std::string root;
                for (size_t i = 0; i < location.block.size(); i++)
                {
                    if (location.block[i].name == "root")
                    {
                        std::cout << "root : " << location.block[i].value << std::endl;
                        root = location.block[i].value;
                    }
                }
                std::cout << "file : " << file << std::endl;
                std::cout << "path : " << root + file << std::endl;
                return &location;
            }
        }
        tmp = tmp.erase(pos);
        pos = tmp.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
    }
    return NULL;
}
