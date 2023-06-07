/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:06 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/05 19:26:42 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "Master.hpp"

Master::Master(int argc, char const *argv[]) : kq(kqueue())
{
	// Create a new kqueue
	if (kq < 0)
		stderrExit("kqueue");

	// Parse the config file
	this->config.parsedConfig(argc, argv);

	// Set the server
	this->server.setServer(this->config, this->events);
	// this->server.printServer();
}

Master::~Master()
{
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

Worker Master::create()
{
	Worker worker(*this);
	return worker;
}
