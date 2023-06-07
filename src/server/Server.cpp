/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:08 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/07 17:28:00 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "commonConfig.hpp"
#include "Utils.hpp"

/*
 * A default constructor
 */
Server::Server() {}

/*
 * A copy constructor
 */
Server::Server(const Server &ref)
{
    this->servers = ref.servers;
}

/*
 * A assignment operator overload
 */
Server &Server::operator=(const Server &ref)
{
    if (this != &ref)
    {
        this->servers = ref.servers;
    }
    return *this;
}

/*
 * A destructor
 */
Server::~Server()
{
    for (std::vector<ServerInfo>::const_iterator it = this->servers.begin(); it != this->servers.end(); it++)
    {
        ServerInfo server = *it;
        server.closeSockets();
    }
}

/**
 * 입력인자를 Server 클래스에 세팅
 *
 * @param config conf 파일을 파싱한 클래스
 */
void Server::setServer(Config &config, std::vector<struct kevent> &events)
{
    std::vector<Directive> server;
    config.getAllDirectives(server, config.getDirectives(), SERVER_DIRECTIVE);
    setUpServer(server, events);
}

/**
 * server 블록 내부에서 listen 지시자를 찾아 포트번호 벡터에 저장. 중복된 포트번호가 존재한다면 에러 반환
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpListen(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == LISTEN_DIRECTIVE)
        {
            int port = strtod(serverBlocks[i].value.c_str(), NULL);
            if (find(this->validPorts.begin(), this->validPorts.end(), port) != this->validPorts.end())
                stderrExit(("Error : duplicate port number " + Utils::ftToString(port) + "\n").c_str());
            tmpServ.ports.push_back(port);
            this->validPorts.push_back(port);
        }
    }
    if (tmpServ.ports.size() != 0)
        return;
    if (find(this->validPorts.begin(), this->validPorts.end(), DEFAULT_PORT) != this->validPorts.end())
        stderrExit("Error : duplicate port number 80\n");
    tmpServ.ports.push_back(DEFAULT_PORT);
    this->validPorts.push_back(DEFAULT_PORT);
}

/**
 * server 블록 내부에서 listen 지시자를 찾아 포트번호 벡터에 저장. 중복된 포트번호가 존재한다면 에러 반환
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpLimitExcept(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == LIMIT_EXCEPT_DIRECTIVE)
        {
            std::vector<std::string> tokens;
            std::istringstream iss(serverBlocks[i].value);
            std::string token;

            while (iss >> token)
                tmpServ.limitExcepts.push_back(token);
        }
    }
}

/**
 * server 블록 내부에서 server_name을 찾아 값을 반환
 *
 * @param serverBlocks 파싱된 서버 블록
 * @return 서버 이름
 */
std::string Server::findServerName(std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == SERVER_NAME_DIRECTIVE)
            return serverBlocks[i].value;
    }
    return "nobody";
}

/**
 * server 블록 내부에서 client_max_body_size 지시자를 찾아 값을 반환
 *
 * @param serverBlocks 파싱된 서버 블록
 * @return 정수 값
 */
size_t Server::findClientMaxBodySize(std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == CLIENT_MAX_BODY_SIZE_DIRECTIVE)
            return static_cast<size_t>(strtod(serverBlocks[i].value.c_str(), NULL));
    }
    return DEFAULT_CLIENT_MAX_BODY_SIZE;
}

/**
 * server 블록 내부에서 root 지시자를 찾아 값을 반환
 *
 * @param serverBlocks 파싱된 서버 블록
 * @return root 경로
 */
std::string Server::findRoot(std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == ROOT_DIRECTIVE)
            return serverBlocks[i].value;
    }
    return "";
}

/**
 * server 블록 내부에서 location 지시자를 찾아 location vector 세팅
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpIndex(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == INDEX_DIRECTIVE)
        {
            tmpServ.index = serverBlocks[i].value;
            return;
        }
    }
    // tmpServ.index = "index.html";
    tmpServ.index = "";
}

/**
 * server 블록 내부에서 error_page 지시자를 찾아 에러페이지 세팅
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == ERROR_PAGE_DIRECTIVE)
        {
            std::vector<std::string> tokens;
            std::istringstream iss(serverBlocks[i].value);
            std::string token;

            while (iss >> token)
                tokens.push_back(token);
            std::string errorPage = tokens[tokens.size() - 1];
            for (size_t j = 0; j < tokens.size() - 1; j++)
            {
                int key = static_cast<int>(strtod(tokens[j].c_str(), NULL));
                tmpServ.errorPage[key] = errorPage;
            }
        }
    }
}

/**
 * server 블록 내부에서 location 지시자를 찾아 location vector 세팅
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpLocation(ServerInfo &tmpServ, std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        if (serverBlocks[i].name == LOCATION_DIRECTIVE)
            tmpServ.locations.push_back(serverBlocks[i]);
    }
}

/**
 * Server 클래스 멤버 변수들을 세팅해주기 위한 메서드
 *
 * @param serverBlocks 파싱된 서버 블록
 */
void Server::setUpServer(std::vector<Directive> &serverBlocks, std::vector<struct kevent> &events)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        ServerInfo server;
        setUpListen(server, serverBlocks[i].block);
        server.serverName = findServerName(serverBlocks[i].block);
        server.clientMaxBodySize = findClientMaxBodySize(serverBlocks[i].block);
        server.root = findRoot(serverBlocks[i].block);
        setUpIndex(server, serverBlocks[i].block);
        setUpErrorPage(server, serverBlocks[i].block);
        setUpLimitExcept(server, serverBlocks[i].block);
        setUpLocation(server, serverBlocks[i].block);
        for (size_t i = 0; i < server.ports.size(); i++)
        {
            int &port = server.ports[i];
            Socket *socket = new Socket(events, port);
            server.sockets.push_back(socket);
        }
        this->servers.push_back(server);
    }
}

/**
 * Server 클래스의 정보 출력
 */
void Server::printServer()
{
    std::cout << "============Server===========" << std::endl;
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        std::cout << "Server[" << i + 1 << "]" << std::endl;
        std::cout << "PORT: ";
        for (size_t j = 0; j < this->servers[i].ports.size(); j++)
            std::cout << this->servers[i].ports[j] << " ";
        std::cout << std::endl;
        std::cout << "Server_name: " << this->servers[i].serverName << std::endl;
        std::cout << "Index: " << this->servers[i].index << std::endl;
        std::cout << "Client_max_body_size: " << this->servers[i].clientMaxBodySize << std::endl;
        std::cout << "Root: " << this->servers[i].root << std::endl;
        for (size_t j = 0; j < this->servers[i].locations.size(); j++)
        {
            std::cout << "location : " << this->servers[i].locations[j].value << std::endl;
        }
        std::cout << "-------------------------------\n";
        for (std::map<int, std::string>::iterator iter = this->servers[i].errorPage.begin(); iter != this->servers[i].errorPage.end(); iter++)
            std::cout << "errorPage : " << iter->first << ", " << iter->second << std::endl;
        std::cout << "===============================\n\n";
    }
}

ServerInfo &Server::findServer(const int &fd)
{
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        ServerInfo &server = this->servers[i];
        std::vector<Socket *> &sockets = server.sockets;
        for (size_t j = 0; j < sockets.size(); j++)
        {
            Socket &socket = *sockets[j];
            if (socket._serverFd == fd)
                return server;
            else
            {
                const std::vector<int> &clientFds = socket._clientFds;
                for (size_t k = 0; k < clientFds.size(); k++)
                {
                    const int clientFd = clientFds[k];
                    if (clientFd == fd)
                        return server;
                }
            }
        }
    }
    // TODO: 예외처리
    return this->servers[0];
}

Socket *Server::findSocket(const int &fd)
{
    for (size_t i = 0; i < this->servers.size(); i++)
    {
        std::vector<Socket *> &sockets = this->servers[i].sockets;
        for (size_t j = 0; j < sockets.size(); j++)
        {
            Socket &socket = *sockets[j];
            if (socket._serverFd == fd)
                return &socket;
            else
            {
                const std::vector<int> &clientFds = socket._clientFds;
                for (size_t k = 0; k < clientFds.size(); k++)
                {
                    const int clientFd = clientFds[k];
                    if (clientFd == fd)
                        return &socket;
                }
            }
        }
    }
    return NULL;
}
