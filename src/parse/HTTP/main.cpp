/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 18:18:25 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/04/24 15:36:17 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

int main()
{
    HTTPRequestParser parser;
    std::string http_request =
        "POST /example/path HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: ExampleBrowser/1.0\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 20\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello World\nGood Bye";
    HTTPRequest *result = parser.parse(http_request);

    if (result)
    {
        std::cout << "Request method: " << (result->method == GET ? "GET" : "POST") << std::endl;
        std::cout << "Request path: " << result->path << std::endl;
        std::cout << "Request HTTP version: " << result->http_version << std::endl;

        for (std::map<std::string, std::string>::const_iterator it = result->headers.begin(); it != result->headers.end(); ++it)
            std::cout << "Header: " << it->first << " = " << it->second << std::endl;

        std::cout << "Body: " << result->body << std::endl;
        delete result;
    }
    else
    {
        std::cout << result << std::endl;
        std::cout << "Failed to parse request" << std::endl;
    }

    return 0;
}
