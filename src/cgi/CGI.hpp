/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/05/17 16:29:28 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>
#include <unistd.h>
#include <cctype>
#include <sys/stat.h>
#include "../parse/HTTPRequestParser.hpp"
#include "../config/Config.hpp"

class CGI
{
private:
	void initEnvp(const HTTPRequest &request);
	char **ENVPChangeStringArray();
	std::string getBody() const;
	void setBody(const std::string &body);
	std::string getResponseBody() const;
	void setEnv(const std::map<std::string, std::string> &env);
	bool isCgiPath(void) const;

	int fileFD_[2];
	std::map<std::string, std::string> envp_;
	std::string cgiPath_;
	std::string body_;
	//
public:
	CGI(const std::string &cgi_path);
	CGI(const HTTPRequest &request);
	CGI(const CGI &src);
	~CGI(void);

	std::string excuteCGI(const std::string &context);
	// void deleteEnvp(char** envp) const; -> 수정 예정
};
#endif
