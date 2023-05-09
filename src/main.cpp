/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/09 17:25:40 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "MimeTypesParser.hpp"
#include "Server.hpp"

int main(int argc, char const *argv[])
{
    // Nginx Config file parsing
    Config config;
    config.parsedConfig(argc, argv);
    config.printDirectives(config.getDirectives(), 0);
    Server server(config);

    // Master master;
    // Worker worker(master);
    // worker.config = config;
    // worker.run();
    return 0;
}
