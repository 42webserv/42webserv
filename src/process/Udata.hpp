/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Udata.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 15:55:05 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 19:15:46 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UDATA_HPP
#define UDATA_HPP

#include <string>
#include "HTTPRequest.hpp"
#include "CookieManager.hpp"

enum EventState
{
	SOCKET_READ,
	SOCKET_WRITE,
	FILE_READ,
	FILE_WRITE,
	END
};

/*
 * event에 같이 들고 다닐 user에 대한 데이터이다.
 *
 * fd: client의 fd
 * keepLive: keep-alive인지 아닌지
 */
struct UData
{
	enum EventState state;
	int max;
	int timeout;
	bool keepAlive;
	bool isClient;
	std::string sessionID;
	bool alreadySessionSend;
	bool sesssionValid;
	std::string expireTime;
	bool wantToDeleteSessionInCookie;
	std::string request; // recv로 받아야 할 문자열
	HTTPRequest *result;
	CookieManager *cookieManager;

	UData(bool keepAlive, enum EventState state);
};

#endif
