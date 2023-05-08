/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 20:43:49 by yje               #+#    #+#             */
/*   Updated: 2023/05/08 16:51:31 by yje              ###   ########.fr       */
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

int main()
{

    std::string filename = "cgi - bin.html";
    std::string filepath = "../../ assets / html /";
    // std::string filepath = “/Users/han/42Seoul/webserv/cgi-bin/“;
    // 파일 경로와 이름을 합칩니다.
    std::string fullpath = filepath + filename;
    // 파일을 열고 문자열을 쓴 후 닫습니다.
    std::ofstream testCGI(fullpath);
    // testCGI << “#!/bin/env python\n”;
    // testCGI << “#!/bin/bash\n”;
    // testCGI << “echo \“Content-Type: text/plain\“\n”;
    // testCGI << “echo \“\”\n”;
    // testCGI << “echo \“Hello, CGI!\“\n”;
    // testCGI.close();
    // Execute test CGI program
    CGI cgi("hello3.py");
    testCGI << cgi.excuteCGI("../../ cgi - bin / hello3.py");
    // Print response body
    std::cout
        << "Response body :\n\n\n\n"
        << cgi.getResponseBody()
        << std::endl;
    // http://52.78.8.100:8088/cgi-bin/multiple.py?a=3&b=5
    //  Remove test CGI program file
    //  remove(” test.cgi”);
    return 0;
}

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

// int main()
// {
//     // Create test CGI program file
//     // testCGI << "#!/bin/bash\n";
//     // testCGI << "echo \"Content-Type: text/plain\"\n";
//     // testCGI << "echo \"\"\n";
//     // testCGI << "echo \"Hello, CGI!\"\n";

//     // 파일 이름과 경로
//     std::string filename = "hello.py";
//     std::string filepath = "/Users/yein/git/webserv/src/cgi-bin/";

//     // 파일 경로와 이름을 합칩니다.
//     std::string fullpath = filepath + filename;

//     // 파일을 열고 문자열을 쓴 후 닫습니다.
//     // std::ofstream testCGI(fullpath);
//     // testCGI << "#!/bin/bash\n";
//     // testCGI << "echo \"Content-Type: text/plain\"\n";
//     // testCGI << "echo \"\"\n";
//     // testCGI << "echo \"Hello, CGI!\"\n";
//     // testCGI.close();

//     // Execute test CGI program
//     CGI cgi("hello.py");
//     // cgi.excuteCGI("./hello.py");
//     cgi.excuteCGI(fullpath);
//     // cgi.excuteCGI("./hello.py");

//     // Print response body
//     std::cout << "body -> "
//               << " Response body :\n "
//               << cgi.getResponseBody()
//               << std::endl;

//     std::ofstream test2CGI("test2.cgi");
//     std::string cgifilename = "test2.html";
//     std::string cgifilepath = "/Users/yein/git/webserv/src/cgi-bin/";
//     std::string fullpath = filepath + filename;
//     test2CGI << cgi.getResponseBody();
//     // testCGI << "echo \"Content-Type: text/plain\"\n";
//     // testCGI << "echo \"\"\n";
//     // testCGI << "echo \"Hello, CGI!\"\n";
//     // Remove test CGI program file
//     // remove(" test.cgi");

//     return 0;
// }
// #include <iostream>
// #include <string>
// #include <cstring>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include "CGI.hpp"

// const int PORT = 8090;

// std::string executeCGI(const std::string& request)
// {
//     // TODO: CGI 실행 및 결과를 반환하는 코드 작성
//     // request를 CGI 함수의 매개변수로 전달하고 CGI 함수의 반환값을 반환해야 합니다.
//     // CGI 함수는 request를 처리하고 결과를 반환하는 로직을 구현해야 합니다.
//     // 필요한 경우 request 파싱 및 환경 변수 설정 등을 수행할 수 있습니다.
//     // 반환값은 CGI 실행 결과를 나타내는 문자열입니다.
// }

// int main(int argc, char const *argv[])
// {
//     int server_fd, new_socket, valread;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);
//     char buffer[1024] = {0};

//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//     {
//         std::cerr << "Socket creation failed" << std::endl;
//         return -1;
//     }

//     // Attaching socket to the port 8080
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT,
//                                                   &opt, sizeof(opt)))
//     {
//         std::cerr << "setsockopt failed" << std::endl;
//         return -1;
//     }
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     // Binding socket to the address and port
//     if (bind(server_fd, (struct sockaddr *)&address,
//                                  sizeof(address)) < 0)
//     {
//         std::cerr << "bind failed" << std::endl;
//         return -1;
//     }

//     // Listening for incoming connections
//     if (listen(server_fd, 3) < 0)
//     {
//         std::cerr << "listen failed" << std::endl;
//         return -1;
//     }

//     // Accepting incoming connections
//     if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//                              (socklen_t *)&addrlen)) < 0)
//     {
//         std::cerr << "accept failed" << std::endl;
//         return -1;
//     }

//     // Receiving request and executing CGI
//     std::string filename = "hello.py";
//     std::string filepath = "/Users/yein/git/webserv/src/cgi-bin/";
//     std::string fullpath = filepath + filename;
//     CGI cgi("hello.py");
//     while (true)
//     {
//         valread = read(new_socket, buffer, 1024);
//         std::cout << buffer << std::endl;

//         // Execute CGI and get the response
//         std::string request(buffer);
//         std::string response = cgi.excuteCGI(fullpath);;

//             std::cout << "Response body:\n"
//               << cgi.getResponseBody() << std::endl;
//         // Send the response back to the client
//         send(new_socket, response.c_str(), response.size(), 0);

//         memset(buffer, 0, sizeof(buffer));
//     }

//     return 0;
// }
