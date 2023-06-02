/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 16:36:22 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/02 21:37:21 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <sys/event.h>
#include "Socket.hpp"
// #define MAX_SIGNAL 7
#define MAX_SIGNAL 3

class Signal
{
private:
	unsigned int signals[MAX_SIGNAL];

public:
	Signal(std::vector<struct kevent> &events);
	~Signal();
	void handleEvent(const int &signal, const std::vector<ServerInfo> &servers) const;
};

#endif
