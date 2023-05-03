/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 16:08:30 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/03 16:08:48 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param request request 를 파싱완료한 구조체
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @return 최종완성된 헤더를 반환함
 */
std::string generateErrorHeader(int status_code, const std::string &message)
{
    std::ostringstream oss;
    // oss << "HTTP/1.1 " << status_code << " " << message << "\r\n";
    oss << "HTTP/1.1 " << status_code << " OK\r\n";
    oss << "Content-Length: " << message.length() << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

/**
 * response의 헤더에 적어줄 내용을 만듬
 *
 * @param request request 를 파싱완료한 구조체
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @return 최종완성된 헤더를 반환함
 */
std::string generateHeader(const HTTPRequest &request, const std::string &content)
{
    HTTPRequestParser parser;
    std::ostringstream oss;

    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Content-Type: " << ((parser.getContentType(request) == "") ? "text/html" : parser.getContentType(request)) << "\r\n"; // MIME type can be changed as needed
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

void errorResponse(int client_fd)
{
    std::string error_path = "./assets/html/404.html";
    std::ifstream error_file(error_path);
    std::string error_content((std::istreambuf_iterator<char>(error_file)),
                              std::istreambuf_iterator<char>());
    std::string error_header = generateErrorHeader(404, error_content);
    write(client_fd, error_header.c_str(), error_header.length());
    write(client_fd, error_content.c_str(), error_content.length());
}

/**
 * response에 보내줄 리소스를 찾는과정
 *
 * @param path uri에서 준 경로
 * @return 리소스를찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 */
void getResponse(const HTTPRequest &request, int client_fd)
{
    // root_dir에 관한내용은 conf에서 가져옴
    std::string root_dir = "./assets/html"; // Root directory for serving static files
    //.ico파일일 경우 임의로 이미지폴더로 이동
    if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
        root_dir = "./assets/images";
    std::string resource_path = root_dir + (request.path == "/" ? "/index.html" : request.path);
    std::ifstream resource_file(resource_path);
    // Resource not found, return a 404 error response
    if (!resource_file.good())
        return errorResponse(client_fd);
    std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
                                 std::istreambuf_iterator<char>());
    std::string response_header = generateHeader(request, resource_content);
    write(client_fd, response_header.c_str(), response_header.length());
    write(client_fd, resource_content.c_str(), resource_content.length());
}

/**
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 서버의 fd
 */
void requestHandler(const HTTPRequest &request, int client_fd)
{
    if (request.method == "GET")
    {
        getResponse(request, client_fd);
    }
    else
    {
        // 잘못된 메서드일경우
        std::string response_body = "Method not allowed";
        std::string response_header = generateErrorHeader(405, response_body);
        write(client_fd, response_header.c_str(), response_header.length());
        write(client_fd, response_body.c_str(), response_body.length());
    }
}
