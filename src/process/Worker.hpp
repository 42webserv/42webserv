/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/04 20:22:20 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"
#include "ParsedConfig.hpp"

class Worker
{
private:
	const int kq;
	const Socket server;
	const Signal signal;
	std::vector<struct kevent> &event_list;

public:
	ParsedConfig config;
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
