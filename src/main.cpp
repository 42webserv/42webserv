/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/04 16:51:02 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "ParsedConfig.hpp"

int main(int argc, char const *argv[])
{
    // Nginx Config file parsing
    ParsedConfig config;
    config.parsedConfig(argc, argv);
    config.printDirectives(config.getDirectives(), 0); // Directive 출력해보기
    std::vector<Directive> servers;
    config.getAllDirectives(servers, config.getDirectives(), "server");
    config.printDirectives(servers, 0);

    Master master;
    if (2 == argc)
        master.setConfig(argv[1]);
    else if (2 < argc)
    {
        std::cout << "Too many arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    Worker worker(master);
    worker.run();
    return 0;
}
