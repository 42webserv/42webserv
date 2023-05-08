/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/04 18:54:19 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Master.hpp"
#include "Worker.hpp"
#include "CGI.hpp"

int main(int, char const *[])
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
    remove("hello.py");

    return 0;
}
