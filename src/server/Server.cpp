/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:08 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/02 19:29:48 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "commonConfig.hpp"

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
}

/**
 * 입력인자를 Server 클래스에 세팅
 *
 * @param config conf 파일을 파싱한 클래스
 */
void Server::setServer(Config &config)
{
    std::vector<Directive> server;
    config.getAllDirectives(server, config.getDirectives(), SERVER_DIRECTIVE);
    setUpServer(server);
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
                stderrExit(("Error : duplicate port number " + std::to_string(port) + "\n").c_str());
            tmpServ.ports.push_back(port);
            this->validPorts.push_back(port);
        }
    }
    if (tmpServ.ports.size() != 0)
        return;
    if (find(this->validPorts.begin(), this->validPorts.end(), 80) != this->validPorts.end())
        stderrExit("Error : duplicate port number 80\n");
    tmpServ.ports.push_back(80);
    this->validPorts.push_back(80);
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
    return 1000000;
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
void Server::setUpServer(std::vector<Directive> &serverBlocks)
{
    for (size_t i = 0; i < serverBlocks.size(); i++)
    {
        ServerInfo tmpServ;
        setUpListen(tmpServ, serverBlocks[i].block);
        tmpServ.serverName = findServerName(serverBlocks[i].block);
        tmpServ.clientMaxBodySize = findClientMaxBodySize(serverBlocks[i].block);
        tmpServ.root = findRoot(serverBlocks[i].block);
        setUpIndex(tmpServ, serverBlocks[i].block);
        setUpErrorPage(tmpServ, serverBlocks[i].block);
        setUpLimitExcept(tmpServ, serverBlocks[i].block);
        setUpLocation(tmpServ, serverBlocks[i].block);
        this->servers.push_back(tmpServ);
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
