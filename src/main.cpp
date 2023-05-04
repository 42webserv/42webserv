/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 17:24:47 by chanwjeo         ###   ########.fr       */
=======
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 15:25:47 by yje              ###   ########.fr       */
>>>>>>> feature/cgi/#8-yein
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
    Worker worker(master);
    worker.run();
    return 0;
}
