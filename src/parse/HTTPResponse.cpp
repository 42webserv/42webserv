/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 16:08:30 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/01 17:48:45 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "HTTPRequestParser.hpp"

// Generate an HTTP response header for an error status code and message.
std::string generate_error_header(int status_code, const std::string &message)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status_code << " " << message << "\r\n";
    oss << "Content-Length: " << message.length() << "\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

std::string get_resource(const std::string &path)
{
    std::string root_dir = "./assets"; // Root directory for serving static files
    // std::string root_dir = "/Users/chanwoong/git/42webserv/assets/html"; // Root directory for serving static files
    // std::cout << "path::: " << path << std::endl;
    // (void)path;
    std::string resource_path = root_dir + path;
    std::ifstream resource_file(resource_path);
    if (!resource_file.good())
    {
        // Resource not found, return a 404 error response
        std::string error_message = "Resource not found";
        std::string error_header = generate_error_header(404, error_message);
        std::cout << "\nnot finded\n";
        return error_header + error_message;
    }
    std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
                                 std::istreambuf_iterator<char>());
    // Resource found, return the content
    return resource_content;
}

// Generate an HTTP response header for a given content length and MIME type.
std::string generate_header(const std::string &content)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Content-Type: text/plain\r\n"; // MIME type can be changed as needed
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

void requestHandler(const HTTPRequest &request, int client_fd)
{
    // 1. Check the HTTP method to determine the action to take.
    if (request.method == GET)
    {
        // 2. Generate a response for the requested resource.
        std::string response_body = get_resource(request.path);
        std::string response_header = generate_header(response_body);
        std::cout << response_header << std::endl;
        // 3. Send the response back to the client.
        write(client_fd, response_header.c_str(), response_header.length());
        write(client_fd, response_body.c_str(), response_body.length());
    }
    else
    {
        // Unsupported HTTP method, send a 405 error response.
        std::string response_body = "Method not allowed";
        std::string response_header = generate_error_header(405, response_body);
        write(client_fd, response_header.c_str(), response_header.length());
        write(client_fd, response_body.c_str(), response_body.length());
    }
}
