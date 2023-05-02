/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 16:08:30 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/02 15:18:03 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

// Generate an HTTP response header for an error status code and message.
std::string generateErrorHeader(int status_code, const std::string &message)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status_code << " " << message << "\r\n";
    oss << "Content-Length: " << message.length() << "\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

// Generate an HTTP response header for a given content length and MIME type.
std::string generateHeader(const std::string &content)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Content-Type: text/html\r\n"; // MIME type can be changed as needed
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

std::string getResource(const std::string &path)
{
    std::string root_dir = "./assets/html"; // Root directory for serving static files
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".ico")
        root_dir = "./assets/images";
    // std::string root_dir = "/Users/chanwoong/git/42webserv/assets/html"; // Root directory for serving static files
    // std::cout << "path::: " << path << std::endl;
    std::string resource_path = root_dir + path;
    std::ifstream resource_file(resource_path);
    if (!resource_file.good())
    {
        // Resource not found, return a 404 error response
        std::string error_message = "Resource not found";
        std::string error_header = generateErrorHeader(404, error_message);
        // std::cout << "\nnot finded\n";
        return error_header + error_message;
    }
    std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
                                 std::istreambuf_iterator<char>());
    // Resource found, return the content
    return resource_content;
}

void requestHandler(const HTTPRequest &request, int client_fd)
{
    // 1. Check the HTTP method to determine the action to take.
    if (request.method == GET)
    {
        // 2. Generate a response for the requested resource.
        std::string response_body = getResource(request.path);
        std::string response_header = generateHeader(response_body);
        // std::cout << response_header << std::endl;
        // 3. Send the response back to the client.
        write(client_fd, response_header.c_str(), response_header.length());
        write(client_fd, response_body.c_str(), response_body.length());
    }
    else
    {
        // Unsupported HTTP method, send a 405 error response.
        std::string response_body = "Method not allowed";
        std::string response_header = generateErrorHeader(405, response_body);
        write(client_fd, response_header.c_str(), response_header.length());
        write(client_fd, response_body.c_str(), response_body.length());
    }
}
