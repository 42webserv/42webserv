/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:43 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/01 15:32:08 by sunhwang         ###   ########.fr       */
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
    ServerInfo &server = serverManger.servers[index];
    ResponseData *response = new ResponseData();
    response->server = server;
    response->index = server.index;
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
    response->contentType = findMimeType(response->resourcePath, config);
    response->body = request.body;
    response->contentLength = response->body.length();
    response->path = getPath(request, response);
    response->resourcePath = response->path;
    return (response);
}

/**
 * url에는 location의 path가 들어올 수도 location의 path 이후 서브 경로가 들어올 수 있다.
 * 또한 url에는 index 값이 포함되어 들어올 수 있다. 마지막에 확장자가 없는 파일도 들어올 수 있다.
 */
std::string Response::getPath(const HTTPRequest &request, ResponseData *response)
{
    // 요청으로 들어온 경로
    std::string routes = request.path;
    // 반환될 변수. 첫 부분은 location에 등록된 root
    std::string path = response->root;
    // location에 등록된 index
    std::string index = response->index;
    bool i = false;
    {
        // location에 등록된 root 값으로 routes에서 지워준다. location root로 대체됨.
        size_t pos = routes.find(response->location->value);
        if (pos != std::string::npos)
            // routes = routes.substr(pos, response->location->value.length() - pos);
            routes = routes.substr(pos + response->location->value.length());
        // location에 등록된 index 값으로 routes에서 지워준다. location index로 대체됨.
        if (!index.empty())
        {
            //에러날경우있음 root/123/index일때 다 123/index가 다 지워짐
            pos = routes.find(index); 
            if (pos != std::string::npos)
                routes = routes.substr(0, pos);
        }
        pos = routes.rfind("/");
        if (pos != std::string::npos)
        {
            size_t extensionPos = pos + 1;
            std::string extension = routes.substr(pos + 1);
            pos = extension.find(".");
            if (pos != std::string::npos)
            {
                routes = routes.substr(0, extensionPos);
                index = extension;
                i = true;
            }
        }
    }
    if (routes.compare(0, response->location->value.length(), response->location->value) != 0)
        path += routes;
    if ((i == false && isDirectory(path)) || i == true)
    {
        path = path + "/" + index;
    }
    std::cout << "============= " << path << std::endl;
    // if (path[path.length() - 1] != '/')
    //     path += "/";
    // path += index;
    return (path);
}

std::string Response::findMimeType(const std::string &path, Config &config)
{
    MimeTypesParser mime(config);
    std::istringstream iss(path);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, '.'))
        tokens.push_back(token);
    std::string extension = tokens.back();
    return mime.getMimeType(extension);
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

    // location 지시문의 value와 request의 path가 일치하는지 확인
    for (std::vector<Directive>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        Directive &location = *it;
        if (location.value == request.path)
            return &location;
    }

    // 일치하지 않는다면, 뒤에서 부터 /의 위치를 찾아 경로를 지우면서 locations을 순회하며 일치하는 location을 찾음
    size_t pos = request.path.rfind('/');
    while (pos != 0 && pos != std::string::npos)
    {
        std::string path = request.path.substr(0, pos);
        for (std::vector<Directive>::iterator it = locations.begin(); it != locations.end(); it++)
        {
            Directive &location = *it;
            if (location.value == path)
            {
                return &location;
            }
        }
        path = path.erase(pos);
        pos = path.rfind('/'); // 이부분 부터는 /를 지우면서 매칭되는 location을 찾음
    }
    return NULL;
}
