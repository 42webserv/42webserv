/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 16:41:37 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/10 22:03:02 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "commonError.hpp"
#include "Server.hpp"
#include "Signal.hpp"
#include "color.hpp"

Signal::Signal(std::vector<struct kevent> &events)
{
	// const unsigned int sigs[MAX_SIGNAL] = {SIGTERM, SIGINT, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2, SIGWINCH};
	const unsigned int sigs[MAX_SIGNAL] = {SIGTERM, SIGINT, SIGQUIT};
	struct kevent event;

	for (size_t i = 0; i < MAX_SIGNAL; i++)
	{
		signals[i] = sigs[i];
		signal(signals[i], SIG_IGN);
		EV_SET(&event, signals[i], EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
		events.push_back(event);
	}
}

Signal::~Signal()
{
	for (size_t i = 0; i < MAX_SIGNAL; i++)
		signal(signals[i], SIG_DFL);
}

void Signal::handleEvent(const int &signal, const std::vector<ServerInfo> &servers) const
{
	for (size_t i = 0; i < MAX_SIGNAL; i++)
	{
		// TERM, INT: 빠른 종료
		if (signal == SIGTERM || signal == SIGINT)
		{
			for (std::vector<ServerInfo>::const_iterator it = servers.begin(); it != servers.end(); it++)
			{
				const ServerInfo &server = *it;
				server.closeSockets();
			}
		    std::cout << BRED "\r🔌 server disconnect " END << std::endl;
			exit(EXIT_SUCCESS);
		}
		// QUIT: 정상적인 종료
		else if (signal == SIGQUIT)
		{
			for (std::vector<ServerInfo>::const_iterator it = servers.begin(); it != servers.end(); it++)
			{
				const ServerInfo &server = *it;
				server.closeSockets();
			}
		    std::cout << BRED "\r🔌 server disconnect " END << std::endl;
			exit(EXIT_SUCCESS);
		}
		// Nginx 미구현 목록
		// HUP: 구성 변경, 변경된 시간대 따라잡기(FreeBSD와 Linux만), 새 구성으로 새 worker processes 시작, 이전 worker processes 정상적인 종료
		// USR1: 로그 파일 다시 열기
		// USR2: 실행 파일 업그레이드
		// WINCH: worker processes의 정상적인 종료
	}
}

/**
 * 키보드 입력 SIGNAL
 * Ctrl-C : SIGINT	(Interrupt)
 * Ctrl-Z : SIGSTP	(Suspend)
 * Ctrl-\ : SIGQUIT	(Quit)
 * Ctrl-T : SIGINFO	(Info)
 */
