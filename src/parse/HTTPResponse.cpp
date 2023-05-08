/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 16:08:30 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/08 15:24:51 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include "MimeTypesParser.hpp"

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
 * @param content getResource함수에서 찾아온 내용을 가져옴
 * @param contentType Content-Type
 * @return 최종완성된 헤더를 반환함
 */
std::string generateHeader(const std::string &content, const std::string &contentType)
{
    HTTPRequestParser parser;
    std::ostringstream oss;

    (void)contentType;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Content-Type: "
        << "html"
        << "\r\n"; // MIME type can be changed as needed
    oss << "Connection: close\r\n\r\n";
    return oss.str();
}

/**
 * 404 에러일 경우 나와야할 페이지 띄워주는 함수
 *
 * @param client_fd 브라우저 포트번호
 */
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
 * GET request일 경우, response에 보내줄 리소스를 찾고 담긴 내용을 가져옴. 파일이 존재하지않으면 에러페이지 반환
 *
 * @param request 파싱된 HTTP 요청 메세지 구조체
 * @param client_fd 웹 소켓
 * @param config 파싱된 config 파일
 */
void getResponse(const HTTPRequest &request, int client_fd, Config &config)
{
    // root_dir에 관한내용은 conf에서 가져옴
    std::string root_dir = "./assets/html"; // Root directory for serving static files
    //.ico파일일 경우 임의로 이미지폴더로 이동
    if (request.path.length() >= 4 && request.path.substr(request.path.length() - 4) == ".ico")
        root_dir = "./assets/images";
    std::string resource_path = root_dir + (request.path == "/" ? "/index.html" : request.path);
    std::ifstream resource_file(resource_path);
    // 리소스를 찾지 못했다면 404페이지로 이동
    if (!resource_file.good())
        return errorResponse(client_fd);

    // 경로에서 확장자 찾아준 뒤, Content-Type 찾기
    std::vector<std::string> tokens;
    std::istringstream iss(resource_path);
    std::string token;
    while (std::getline(iss, token, '.'))
        tokens.push_back(token);
    std::string extension = tokens.back();
    MimeTypesParser mime(config);
    std::string contentType = mime.getMimeType(extension);

    std::string resource_content((std::istreambuf_iterator<char>(resource_file)),
                                 std::istreambuf_iterator<char>());
    std::string response_header = generateHeader(resource_content, contentType);
    write(client_fd, response_header.c_str(), response_header.length());
    write(client_fd, resource_content.c_str(), resource_content.length());
}

/**
 * 각각 method 실행과 해당 포트에 response를 보내줌
 *
 * @param request request 를 파싱완료한 구조체
 * @param client_fd 서버의 fd
 * @param config 파싱된 config 파일
 */
void requestHandler(const HTTPRequest &request, int client_fd, Config &config)
{
    if (request.method == "GET")
    {
        getResponse(request, client_fd, config);
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
