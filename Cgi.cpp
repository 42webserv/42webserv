/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:29:58 by yje               #+#    #+#             */
/*   Updated: 2023/04/28 19:22:02 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void CGI::CGI()
{

}

void	CGI::initEnvp()//request config 이름 확인해서 받아오기
{
	// std::map<std::string, std::string> HTTPRequest.headers;//HTTPRequestParser확인하기

	const std::string& method = HTTPRequest.method;
	std::size_t content_length = request.getContentLength(); //contentlength
  	// if (method == "POST" && content_length > 0)
	// {
    // 	headers["CONTENT_LENGTH"] = toString(content_length);
	// }
	this->_env["AUTH_TYPE"] = ""
	// this->_env["CONTENT_LENGTH"] = to_string(this->_body.length());
	this->_env["CONTENT_TYPE"] = request.getHeader("CONTENT-TYPE"); //헤더가져오기
	this->_env["GATEWAY_INTERFACE"]	= "CGI/1.1";
	// this->_env["HTTP_ACCEPT"] = ""
	this->_env["HTTP_ACCEPT_CHARSET"]=



};


CGI::getenv()
{

}
