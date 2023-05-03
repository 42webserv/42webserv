/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/23 18:18:25 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/04/26 16:36:54 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

int main()
{
    HTTPRequestParser parser;
    std::string http_request_post =
        "POST /example/path?request HTTP/1.1\r\n"
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
    std::string http_request_get =
        "GET /example/path HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: ExampleBrowser/1.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
    HTTPRequest *result = parser.parse(http_request_post);
    // HTTPRequest *result = parser.parse(http_request_get);

    if (result)
    {
        parser.printResult(*result);
        delete result;
    }
    else
    {
        std::cout << "Failed to parse request" << std::endl;
    }

    return 0;
}
