/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:43 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/25 19:53:06 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonConfig.hpp"
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
ResponseData *Response::getResponseData(const HTTPRequest &request, const int &client_fd, Config &config)
{
    ResponseData *response = new ResponseData;
    response->index = this->thisServer.index;
    response->path = request.path;
    response->method = request.method;
    response->clientFd = client_fd;
    response->location = this->thisServer.location;
    response->root = getRootDirectory(request, thisServer);
    int idx = matchLocation(request, thisServer);
    if (idx != -1)
    {
        size_t i = static_cast<size_t>(idx);
        setUpRoot(this->thisServer.location[i].block, response);
        setUpIndex(this->thisServer.location[i].block, response);
        setUpAutoindex(this->thisServer.location[i].block, response);
        setUpLimitExcept(this->thisServer.location[i].block, response);
        setUpReturnState(this->thisServer.location[i].block, response);
    }
    if (response->limitExcept.size() == 0)
        response->limitExcept = thisServer.limitExcept;
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
            response->root = locationBlock[i].value;
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
            response->index = locationBlock[i].value;
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
            return;
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
std::string Response::getRootDirectory(const HTTPRequest &request, const ServerInfo &thisServer)
{
    //.ico파일일 경우 임의로 이미지폴더로 이동
    if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ICO_EXTENSION)
        return "./assets/images"; // TODO image 경로 지정하는 방법 찾아보기
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
    size_t pos = request.path.rfind('/'); // 처음엔 확장자만 지워서 매칭되는 location을 찾음
    while (pos != std::string::npos)
    {
        std::string tmp = request.path.substr(0, pos);
        for (size_t i = 0; i < thisServer.location.size(); ++i)
        {
            if (thisServer.location[i].value == tmp)
                return static_cast<int>(i);
        }
        tmp = tmp.erase(pos);
        pos = tmp.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
    }
    return -1;
}
