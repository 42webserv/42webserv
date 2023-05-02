/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:29:58 by yje               #+#    #+#             */
/*   Updated: 2023/05/01 18:37:18 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

// void CGI::CGI()
// {
// }

void CGI::initEnvp() // request config 이름 확인해서 받아오기
{
	// std::map<std::string, std::string> HTTPRequest.headers;//HTTPRequestParser확인하기

	const std::string &method = HTTPRequest.method;
	std::size_t content_length = request.getContentLength(); // contentlength
	// if (method == "POST" && content_length > 0)
	// {
	// 	headers["CONTENT_LENGTH"] = toString(content_length);
	// }
	this->envp_["AUTH_TYPE"] = "";
	// this->envp_["CONTENT_LENGTH"] = to_string(this->_body.length());
	this->envp_["CONTENT_TYPE"] = request.getHeader("CONTENT-TYPE"); // 헤더가져오기
	this->envp_["GATEWAY_INTERFACE"] = "CGI/1.1";
	// this->envp_["HTTP_ACCEPT"] = ""
	this->envp_["PATH_INFO"] = HTTPRequest.path;
	this->envp_["PATH_TRANSLATED"] = HTTPRequest.path;
	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑.
	//  요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	this->envp_["QUERY_STRING"] = request.getQueryString(); // 쿼리스트링
	// this->envp_["REMOTE_ADDR"] = ""ip주소 받아와야 함
	this->envp_["REMOTE_IDENT"] = ""; //-> 권한 부여
	this->envp_["REMOTE_USER"] = "";
	this->envp_["REQUEST_METHOD"] = method;
	this->envp_["REQUEST_URI"] = ""; // request.getUri();
	this->envp_["SCRIPT_NAME"] = request.getUri();
	this->envp_["SERVER_NAME"] = ""; // 요청을 수신한 서버의 호스트 이름. 
	this->envp_["SERVER_PORT"] = ""; // 요청을 수신한 서버의 포트 번호.
	this->envp_["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->envp_["SERVER_SOFTWARE"] = "webserv/1.1";
};

// CGI::getenv()
// {

// }
