/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:09:59 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/26 10:31:37 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORKER_HPP
#define WORKER_HPP

#include "Master.hpp"
#include "Signal.hpp"
#include "Socket.hpp"

class Worker
{
private:
	const int kq;
	const Socket server;
	const Signal signal;
	std::vector<struct kevent> &event_list;

public:
	Worker(Master &master);
	~Worker();
	void run();
};

#endif
