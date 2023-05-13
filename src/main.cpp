/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/11 12:41:06 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "server/Server.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "CheckConfigValid.hpp"
#include "MimeTypesParser.hpp"

void leaks()
{
    system("leaks webserv");
}

int main(int argc, char const *argv[])
{
    // atexit(leaks);
    Master master(argc, argv);
    Worker worker(master);
    worker.run();
    return 0;
}
