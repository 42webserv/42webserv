/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:14 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/10 14:22:58 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTER_HPP
#define MASTER_HPP

#include <sys/event.h>
#include <vector>
#include "Config.hpp"
#include "Config.hpp"
#include "Server.hpp"

class Master
{
private:
	Config config;
	Server server;
	std::vector<struct kevent> events;

public:
	const int kq;
	Master(int argc, char const *argv[]);
	~Master();
	std::vector<struct kevent> &getEvents();
	Config &getConfig();
	Server &getServer();
};

#endif
