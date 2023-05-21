/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:06 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/21 19:44:23 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "Master.hpp"
#include "DefaultConfig.hpp"

Master::Master(int argc, char const *argv[]) : kq(kqueue())
{
	// Create a new kqueue
	if (kq < 0)
	{
		perror("kqueue");
		exit(EXIT_FAILURE);
	}

	// Parse the config file
	this->config.parsedConfig(argc, argv);
	DefaultConfig defaultConfig(this->config); // TODO config 안에다 추가할 수 있을지 확인해보기
	defaultConfig.checkDirectives();

	// Set the server
	this->server.setServer(this->config);
	// this->server.printServer();
}

Master::~Master()
{
	// delete config;
	close(kq);
}

std::vector<struct kevent> &Master::getEvents()
{
	return events;
}

Config &Master::getConfig()
{
	return config;
}

Server &Master::getServer()
{
	return server;
}
