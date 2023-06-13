/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Udata.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 15:55:39 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 18:13:26 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Udata.hpp"

UData::UData(bool keepAlive, enum EventState state)
{
	this->state = state;
	this->keepAlive = keepAlive;
	this->max = -1;
	this->timeout = -1;
	this->sessionID = "";
	this->alreadySessionSend = false;
	this->sesssionValid = false;
	this->expireTime = "";
	this->wantToDeleteSessionInCookie = false;
	this->request = "";
	this->result = NULL;
}
