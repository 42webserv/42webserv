/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 16:36:22 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 16:26:56 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <sys/event.h>
#include "Socket.hpp"
#define MAX_SIGNAL 7

class Signal
{
private:
	unsigned int signals[MAX_SIGNAL];

public:
	Signal(std::vector<struct kevent> &event_list);
	~Signal();
	void handleEvent(const struct kevent &event, const Socket &socket) const;
};

#endif
