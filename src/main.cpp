/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/07 00:02:15 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "Config.hpp"
#include "MimeTypesParser.hpp"

int main(int argc, char const *argv[])
{
    // Nginx Config file parsing
    Config config;
    config.parsedConfig(argc, argv);
    config.printDirectives(config.getDirectives(), 0);
    Master master;
    Worker worker(master);
    worker.config = config;
    worker.run();
    return 0;
}
