/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 16:11:08 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/09 20:40:01 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
 * A default constructor
 */
Server::Server() {}

/*
 * A copy constructor
 */
Server::Server(const Server &ref)
{
    this->server = ref.server;
}

/*
 * A assignment operator overload
 */
Server &Server::operator=(const Server &ref)
{
    if (this != &ref)
    {
        this->server = ref.server;
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
    config.getAllDirectives(server, config.getDirectives(), "server");
    setUpServer(server);
}

/**
 * server 블록 내부에서 listen 지시자를 찾아 포트번호를 반환
 *
 * @param serverBlock 파싱된 서버 블록
 * @return 포트번호
 */
int Server::findListen(std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        if (serverBlock[i].name == "listen")
            return strtod(serverBlock[i].value.c_str(), NULL);
    }
    return 80;
}

/**
 * server 블록 내부에서 server_name을 찾아 값을 반환
 *
 * @param serverBlock 파싱된 서버 블록
 * @return 서버 이름
 */
std::string Server::findServerName(std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        if (serverBlock[i].name == "server_name")
            return serverBlock[i].value;
    }
    return "nobody";
}

/**
 * server 블록 내부에서 client_max_body_size 지시자를 찾아 값을 반환
 *
 * @param serverBlock 파싱된 서버 블록
 * @return 정수 값
 */
size_t Server::findClientMaxBodySize(std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        if (serverBlock[i].name == "client_max_body_size")
            return static_cast<size_t>(strtod(serverBlock[i].value.c_str(), NULL));
    }
    return -1;
}

/**
 * server 블록 내부에서 root 지시자를 찾아 값을 반환
 *
 * @param serverBlock 파싱된 서버 블록
 * @return root 경로
 */
std::string Server::findRoot(std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        if (serverBlock[i].name == "root")
            return serverBlock[i].value;
    }
    return "";
}

/**
 * server 블록 내부에서 error_page 지시자를 찾아 에러페이지 세팅
 *
 * @param tmpServ 현재 서버 정보를 저장할 구조체
 * @param serverBlock 파싱된 서버 블록
 */
void Server::setUpErrorPage(ServerInfo &tmpServ, std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        if (serverBlock[i].name == "error_page")
        {
            std::vector<std::string> tokens;
            std::istringstream iss(serverBlock[i].value);
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
 * Server 클래스 멤버 변수들을 세팅해주기 위한 메서드
 *
 * @param serverBlock 파싱된 서버 블록
 */
void Server::setUpServer(std::vector<Directive> &serverBlock)
{
    for (size_t i = 0; i < serverBlock.size(); i++)
    {
        ServerInfo tmpServ;
        tmpServ.port = findListen(serverBlock[i].block);
        tmpServ.serverName = findServerName(serverBlock[i].block);
        tmpServ.clientMaxBodySize = findClientMaxBodySize(serverBlock[i].block);
        tmpServ.root = findRoot(serverBlock[i].block);
        setUpErrorPage(tmpServ, serverBlock[i].block);
        for (size_t j = 0; j < serverBlock[i].block.size(); j++)
        {
            if (serverBlock[i].block[j].name == "location")
                tmpServ.location.push_back(serverBlock[i].block[j]);
        }
        this->server.push_back(tmpServ);
    }
}

/**
 * Server 클래스의 정보 출력
 */
void Server::printServer()
{
    std::cout << "============Server===========" << std::endl;
    for (size_t i = 0; i < this->server.size(); i++)
    {
        std::cout << "Server[" << i + 1 << "]" << std::endl;
        std::cout << "PORT: " << this->server[i].port << std::endl;
        std::cout << "Server_name: " << this->server[i].serverName << std::endl;
        std::cout << "Client_max_body_size: " << this->server[i].clientMaxBodySize << std::endl;
        std::cout << "Root: " << this->server[i].root << std::endl;
        for (size_t j = 0; j < this->server[i].location.size(); j++)
        {
            std::cout << "location : " << this->server[i].location[j].value << std::endl;
        }
        std::cout << "-------------------------------\n";
        for (std::map<int, std::string>::iterator iter = this->server[i].errorPage.begin(); iter != this->server[i].errorPage.end(); iter++)
            std::cout << "errorPage : " << iter->first << ", " << iter->second << std::endl;
        std::cout << "===============================\n\n";
    }
}
