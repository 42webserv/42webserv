/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 16:41:37 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:24:13 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "commonError.hpp"
#include "Signal.hpp"

Signal::Signal(std::vector<struct kevent> &event_list)
{
	const unsigned int sigs[MAX_SIGNAL] = {SIGTERM, SIGINT, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2, SIGWINCH};
	struct kevent event;

	for (size_t i = 0; i < MAX_SIGNAL; i++)
	{
		signals[i] = sigs[i];
		signal(signals[i], SIG_IGN);
		EV_SET(&event, signals[i], EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
		event_list.push_back(event);
	}
}

Signal::~Signal()
{
	for (size_t i = 0; i < MAX_SIGNAL; i++)
		signal(signals[i], SIG_DFL);
}

void Signal::handleEvent(const struct kevent &event, std::vector<Socket *> &sockets) const
{
	for (size_t i = 0; i < sockets.size(); i++)
		delete sockets[i];
	exit(EXIT_SUCCESS);
	int sig = event.ident;
	// TODO 디버깅으로 돌리면 종료되지 않음. 왜 그런지 확인 필요
	for (size_t i = 0; i < MAX_SIGNAL; i++)
	{
		// TERM, INT: 빠른 종료
		if (sig == SIGTERM || sig == SIGINT)
		{
			for (size_t i = 0; i < sockets.size(); i++)
				delete sockets[i];
			exit(EXIT_SUCCESS);
		}
		// QUIT: 정상적인 종료
		else if (sig == SIGQUIT)
		{
			// for // 모든 애들 소멸하도록 고쳐야함
			for (size_t i = 0; i < sockets.size(); i++)
				delete sockets[i];
			exit(EXIT_SUCCESS);
		}
		// 미구현 목록
		// HUP: 구성 변경, 변경된 시간대 따라잡기(FreeBSD와 Linux만), 새 구성으로 새 worker processes 시작, 이전 worker processes 정상적인 종료
		// USR1: 로그 파일 다시 열기
		// USR2: 실행 파일 업그레이드
		// WINCH: worker processes의 정상적인 종료

		// else if (sig == SIGUSR1)
		// else if (sig == SIGUSR2)
		// else if (sig == SIGWINCH)
	}
}

/**
 * 키보드 입력 SIGNAL
 * Ctrl-C : SIGINT	(Interrupt)
 * Ctrl-Z : SIGSTP	(Suspend)
 * Ctrl-\ : SIGQUIT	(Quit)
 * Ctrl-T : SIGINFO	(Info)
 */
