/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 21:10:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/27 14:24:53 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/event.h>
#include <unistd.h>
#include "common_error.hpp"
#include "HTTPRequestParser.hpp"
#include "Worker.hpp"

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
					// TODO 요청에 대한 처리는 이 부분에서 처리하면 될 것 같다.
					clients[client_fd] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!\n";
				}
				else if (clients.find(fd) != clients.end())
				{
					char buf[1024];
					int n = read(fd, buf, sizeof(buf));
					if (n < 1)
					{
						if (n < 0)
							std::cerr << "Client read error!" << '\n';
						HTTPRequest *result = parser.parse(clients[fd]);
						if (result)
						{
							parser.printResult(*result);
							delete result;
						}
						else
							std::cout << "Failed to parse request" << std::endl;
						server.disconnect_client(fd, clients);
					}
					else
					{
						buf[n] = '\0';
						clients[fd] += buf;
						std::cout << "Received data from " << fd << ": " << clients[fd] << std::endl;
					}
				}
			}
			else if (event.filter == EVFILT_WRITE)
			{
				if (clients.find(fd) != clients.end())
				{
					std::cout << "clients[fd]: " << clients[fd] << std::endl;

					int n = write(fd, clients[fd].c_str(), clients[fd].size());
					if (n == -1)
					{
						std::cerr << "Client write error!" << '\n';
						server.disconnect_client(fd, clients);
					}
					else
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
