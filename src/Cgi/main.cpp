/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 20:43:49 by yje               #+#    #+#             */
/*   Updated: 2023/05/04 18:02:26 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Master.hpp"
// #include "Worker.hpp"

// int main(int, char const *[])
// {
//     Master master;
//     Worker worker(master);
//     worker.run();

//     return 0;
// }

// #include <iostream>
// #include "CGI.hpp"

// int main()
// {
// 테스트를 위한 임의의 환경 변수
// std::map<std::string, std::string> env;
// env["KEY1"] = "VALUE1";
// env["KEY2"] = "VALUE2";
// env["KEY3"] = "VALUE3";

// CGI 객체 생성 및 envp_ 멤버 변수 설정
// init 확인
//     CGI cgi;
//     cgi.initEnvp();

//     // ENVPChangeStringArray() 함수 테스트
//     char **envp = cgi.ENVPChangeStringArray();
//     for (int i = 0; envp[i] != NULL; i++)
//     {
//         std::cout << envp[i] << std::endl;
//         delete[] envp[i];
//     }
//     delete[] envp;

//     return 0;
// }

// #include <iostream>
// #include <sstream>
// #include <map>
// #include "CGI.hpp"

// int main()
// {
//     // create CGI object
//     CGI cgi;

//     // set environment variables
//     std::map<std::string, std::string> envp;
//     envp["REQUEST_METHOD"] = "GET";
//     envp["QUERY_STRING"] = "id=1234&name=john";
//     envp["CONTENT_TYPE"] = "text/html";
//     cgi.setEnv(envp);
//     // prepare request body

//     std::stringstream ss;
//     ss << "id=1234&name=john";
//     std::string requestBody = ss.str();

//     // execute CGI and print output
//     try
//     {
//         cgi.excuteCGI(requestBody);
//         std::cout << cgi.getBody() << std::endl; // print CGI output
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     return 0;
// }

#include <iostream>
#include <fstream>
#include "CGI.hpp"

// int main()
// {
//     // Create test CGI program file
//     // std::ofstream testCGI("test.cgi");
//     // testCGI << "#!/bin/bash\n";
//     // testCGI << "echo \"Content-Type: text/plain\"\n";
//     // testCGI << "echo \"\"\n";
//     // testCGI << "echo \"Hello, CGI!\"\n";

//     // 파일 이름과 경로
//     std::string filename = "test.cgi";
//     std::string filepath = "/Users/yein/git/webserv/src/Cgi/";

//     // 파일 경로와 이름을 합칩니다.
//     std::string fullpath = filepath + filename;

//     // 파일을 열고 문자열을 쓴 후 닫습니다.
//     std::ofstream outfile(fullpath);
//     outfile << "Hello, world!";
//     outfile.close();

//     // Execute test CGI program
//     CGI cgi("test.cgi");
//     cgi.excuteCGI("./test.cgi");

//     // Print response body
//     std::cout << "Response body:\n"
//               << cgi.getResponseBody() << std::endl;

//     // Remove test CGI program file
//     // remove("test.cgi");

//     return 0;
// }

int main()
{
    // Create test CGI program file
    // std::ofstream testCGI("test.cgi");
    // testCGI << "#!/bin/bash\n";
    // testCGI << "echo \"Content-Type: text/plain\"\n";
    // testCGI << "echo \"\"\n";
    // testCGI << "echo \"Hello, CGI!\"\n";

    // 파일 이름과 경로
    std::string filename = "hello.py";
    std::string filepath = "/Users/yein/git/webserv/src/Cgi/";

    // 파일 경로와 이름을 합칩니다.
    std::string fullpath = filepath + filename;

    // 파일을 열고 문자열을 쓴 후 닫습니다.
    // std::ofstream testCGI(fullpath);
    // testCGI << "#!/bin/bash\n";
    // testCGI << "echo \"Content-Type: text/plain\"\n";
    // testCGI << "echo \"\"\n";
    // testCGI << "echo \"Hello, CGI!\"\n";
    // testCGI.close();

    // Execute test CGI program
    CGI cgi("hello.py");
    // cgi.excuteCGI("./hello.py");
    // cgi.excuteCGI(fullpath);
    cgi.excuteCGI("./hello.py");

    // Print response body
    std::cout << "Response body:\n"
              << cgi.getResponseBody() << std::endl;

    // Remove test CGI program file
    // remove(" test.cgi");

    return 0;
}