/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:06 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/04 15:03:49 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "Master.hpp"
#include "DefaultConfig.hpp"
#include "ParsedConfig.hpp"

Master::Master() : kq(kqueue())
{
	// Create a new kqueue
	if (kq < 0)
	{
		perror("kqueue");
		exit(EXIT_FAILURE);
	}
	config = new DefaultConfig();
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

void Master::setConfig(std::string path)
{
	(void)path;
	Config *temp = config;
	// config = new ParsedConfig(config);
	delete temp;
}