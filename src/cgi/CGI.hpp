/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/05/30 22:21:21 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <cctype>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include "Config.hpp"
#include "HTTPRequestParser.hpp"

#define R 0
#define W 1

class CGI
{
private:
	void initEnvp(const HTTPRequest &request);
	char **ENVPChangeStringArray();
	// std::string getBody() const;
	// void setBody(const std::string &body);
	// std::string getResponseBody() const;
	// void setEnv(const std::map<std::string, std::string> &env);
	void childProcess(const int fileFds[2], const std::string &program, char **envp);
	void parentProcess(const pid_t &pid, const int fileFds[2], std::string &body);
	// bool isCgiPath(void) const;

	std::map<std::string, std::string> envp_;
	// std::string cgiPath_;
	std::string body_;

public:
	CGI(const HTTPRequest &request);
	~CGI(void);

	std::string excuteCGI(const std::string &context);
	// void deleteEnvp(char** envp) const; -> 수정 예정
};
#endif
