/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 15:00:33 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include "HTTPRequestParser.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "NginxConfig.hpp"

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    // Nginx Config file parsing
    if (argc != 1 && argc != 2)
    {
        std::cout << "Usage: ./webserv [config_file]" << std::endl;
        exit(1);
    }
    Config config;
    if (argc == 2)
    {
        if (!CheckConfigValid::Parse(argv[1]))
        {
            std::cout << "Error: Invalid config file" << std::endl;
            exit(1);
        }
        config.loadFromFile(argv[1]);
    }
    else
    {
        if (!CheckConfigValid::Parse("src/config/default.conf"))
        {
            std::cout << "Error: Invalid config file" << std::endl;
            exit(1);
        }
        config.loadFromFile("src/config/default.conf");
    }

    std::cout << "Config: " << std::endl;
    config.printConfig(config.getDirectives(), 0);

    Master master;
    Worker worker(master);
    worker.run();

    HTTPRequestParser parser;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    read(new_socket, buffer, 1024); // buffer에 클라이언트가 보낸 메시지를 저장
    send(new_socket, hello, strlen(hello), 0);
    HTTPRequest *result = parser.parse(buffer);

    if (result)
    {
        parser.printResult(*result);
        delete result;
    }
    else
    {
        std::cout << "Failed to parse request" << std::endl;
    }
    return 0;
}
