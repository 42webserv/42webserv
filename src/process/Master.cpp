/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:06 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 21:16:25 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "Master.hpp"
#include "DefaultConfig.hpp"
#include "ParsedConfig.hpp"

Master::Master(int argc, char const *argv[]) : kq(kqueue())
{
	// Create a new kqueue
	if (kq < 0)
	{
		perror("kqueue");
		exit(EXIT_FAILURE);
	}
	this->config.parsedConfig(argc, argv);
	this->server.setServer(this->config);
	this->server.printServer();
}

Master::~Master()
{
	delete config;
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
