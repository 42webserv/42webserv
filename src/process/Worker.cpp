/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 15:06:41 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/event.h>
#include <unistd.h>
#include "common_error.hpp"
#include "HTTPRequestParser.hpp"
#include "Worker.hpp"
#include "HTTPResponse.hpp"

Worker::Worker(Master &master) : kq(master.kq), server(master.getEvents()), signal(master.getEvents()), event_list(master.getEvents()) {}

Worker::~Worker() {}

void Worker::run()
{
	struct kevent events[10];
	struct kevent event;
	int nevents;
	int fd;
	std::map<int, std::string> clients;
	HTTPRequestParser parser;

	while (true)
	{
		// event_list	-> events we want to monitor
		// event		-> events that were triggered
		nevents = kevent(kq, &event_list[0], event_list.size(), events, 10, NULL);
		if (nevents == -1)
		{
			std::cerr << "Error waiting for events: " << strerror(errno) << std::endl;
			break;
		}
		event_list.clear();

		for (int i = 0; i < nevents; i++)
		{
			event = events[i];
			fd = event.ident;
			if (event.flags & EV_ERROR)
			{
				// handle error
				if (fd == server.server_fd)
					// 서버 소켓 에러
					error_exit("Server socket error");
				else
				{
					// 클라이언트 소켓 에러 아니면 다른 에러
					if (clients.find(fd) != clients.end())
						server.disconnect_client(fd, clients);
				}
			}
			if (event.filter == EVFILT_READ)
			{
				if (fd == server.server_fd)
				{
					int client_fd = server.handle_event(event_list);
					clients[client_fd].clear();
				}
				else if (clients.find(fd) != clients.end())
				{
					char buf[1024];
					int n = 1;
					while (0 < (n = read(fd, buf, sizeof(buf))))
					{

						buf[n] = '\0';
						clients[fd] += buf;
						std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;
					}
					if (n < 1)
					{
						if (n < 0)
							std::cerr << "Client read error!" << '\n';
						struct kevent new_event;
						EV_SET(&new_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
						event_list.push_back(new_event);
					}
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				if (clients.find(fd) != clients.end())
				{
					// std::cout << "clients[fd]: " << clients[fd] << std::endl;

					HTTPRequest *result = parser.parse(clients[fd]);
					if (result)
					{
						// parser.printResult(*result);
						// TODO: HTTP Response 구현
						requestHandler(*result, fd);
						delete result;
					}
					else
						std::cout << "Failed to parse request" << std::endl;
					server.disconnect_client(fd, clients);
					clients[fd].clear();
				}
				// 큰 파일 처리할 때
				// off_t offset = (off_t)event.udata;
				// off_t len = 0;
				// if (sendfile(junk, fd, offset, &len, NULL, 0) != 0)
				// {
				// 	if (errno == EAGAIN)
				// 	{
				// 		EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, (void *)(offset + len));
				// 		event_list.push_back(event);
				// 	}
				// }
				// bytes_written += len;
			}
			else if (event.filter == EVFILT_SIGNAL)
				signal.handle_event(event, server);
		}
	}
}
