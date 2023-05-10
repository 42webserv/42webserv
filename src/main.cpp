/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/10 17:28:22 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "CGI.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "MimeTypesParser.hpp"
#include "Server.hpp"

int main(int argc, char const *argv[])
{
    std::string filename = "hello.py";
    std::string filepath = "/usr/bin/python3/";
    std::string fullpath = filepath + filename;

    CGI cgi("hello.py");
    cgi.excuteCGI("./hello.py");
    // cgi.excuteCGI("./hello.py");

    // std::cout << "Response body:\n"
    //   << cgi.getResponseBody() << std::endl;
    // Master master;
    // Worker worker(master);
    // worker.run();
    // remove("hello.py");

    // Nginx Config file parsing
    Config config;
    config.parsedConfig(argc, argv);
    config.printDirectives(config.getDirectives(), 0);
    Server server(config);

    Master master;
    Worker worker(master);
    worker.config = config;
    worker.run();
    return 0;
}
