/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 20:31:06 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/24 20:25:24 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "Master.hpp"

Master::Master() : kq(kqueue())
{
	// Create a new kqueue
	if (kq < 0)
	{
		perror("kqueue");
		exit(EXIT_FAILURE);
	}
}

Master::~Master()
{
	close(kq);
}

std::vector<struct kevent> &Master::getEvents()
{
	return events;
}
