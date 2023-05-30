/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:29:58 by yje               #+#    #+#             */
/*   Updated: 2023/05/30 17:49:38 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parse/HTTPRequestParser.hpp"
#include "CGI.hpp"
// CGI 환경변수 세팅

CGI::CGI(const HTTPRequest &request) : cgiPath_(""), body_(request.body)
{
	initEnvp(request);
}

CGI::CGI(const std::string &cgiPath)
	: cgiPath_(cgiPath)
{
}

CGI::~CGI() {}

void CGI::initEnvp(const HTTPRequest &request) // request config 이름 확인해서 받아오기
{
	// std::map<std::string, std::string> HTTPRequest.headers;//HTTPRequestParser확인하기
	HTTPRequestParser request_parser;
	// const std::string &method = request.method;
	//  std::size_t content_length = request.getContentLength(); // contentlength
	//  if (method == "POST" && content_length > 0)
	//  {
	//  	headers["CONTENT_LENGTH"] = toString(content_length);
	//  }
	this->envp_["AUTH_TYPE"] = "";
	this->envp_["CONTENT_LENGTH"] = std::to_string(request.body.length());
	this->envp_["CONTENT_TYPE"] = request_parser.getContentType(request);
	this->envp_["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->envp_["PATH_INFO"] = request.path;
	this->envp_["PATH_TRANSLATED"] = request.path;
	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑.
	//  요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	this->envp_["QUERY_STRING"] = request.query;
	this->envp_["REMOTE_ADDR"] = request.addr;
	this->envp_["REMOTE_IDENT"] = ""; //-> 권한 부여
	this->envp_["REMOTE_USER"] = "";
	this->envp_["REQUEST_METHOD"] = request.method;
	this->envp_["REQUEST_URI"] = request.name; //
	this->envp_["SCRIPT_NAME"] = request.name; //
	// this->envp_["SERVER_NAME"] = config._server.; // 요청을 수신한 서버의 호스트 이름.
	this->envp_["SERVER_PORT"] = request.strPort; // 요청을 수신한 서버의 포트 번호.
	this->envp_["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->envp_["SERVER_SOFTWARE"] = "webserv/1.1";
};

std::string CGI::getBody() const
{
	return body_;
}

void CGI::setBody(const std::string &body)
{
	this->body_ = body;
}

std::string CGI::getResponseBody() const
{
	return this->body_;
}

void CGI::setEnv(const std::map<std::string, std::string> &envp)
{
	this->envp_ = envp;
}

bool CGI::isCgiPath(void) const
{
	char *cgiPath = const_cast<char *>(cgiPath_.c_str());
	// const char *filepath = const_cast<char *>(cgiPath_.c_str()); > filepath를 이용하는 경우 사용
	if (access(cgiPath, X_OK) == -1)
	{
		return false;
	}
	return true;
}

char **CGI::ENVPChangeStringArray()
{
	char **envp = new char *[this->envp_.size() + 1];
	int idx = 0;
	for (std::map<std::string, std::string>::const_iterator it = this->envp_.begin(); it != this->envp_.end(); it++)
	{
		std::string element = it->first + "=" + it->second;
		envp[idx] = new char[element.size() + 1];
		envp[idx] = strcpy(envp[idx], (const char *)element.c_str());
		idx++;
	}
	envp[idx] = NULL;
	return envp;
}

/**
 * cgi 실행
 *
 * @param file[2] 두 개의 파일 포인터를 저장하기 위한 변수입니다. file[0]은 파이프의 읽기(read), file[1]은 쓰기(write)을 나타냅니다.
 * @param pid: 새로운 프로세스를 생성하기 위한 변수. fork() 함수를 사용하여 현재 프로세스의 복제본을 만들고, 부모 프로세스와 자식 프로세스를 나누게 됩니다.
 * @param oldFD 이전에 열렸던 파일 디스크립터(file descriptor)를 저장하는 변수.
 * @param filefd[2] 새로운 파일 디스크립터를 저장하는 변수입니다. pipe() 함수를 사용하여 파이프를 열면, 새로운 파일 디스크립터가 반환.
 */

std::string CGI::excuteCGI(const std::string &context, const HTTPRequest &request) // context 받기 아마두 경로?
{
	// this->_body = HTTPRequest.body_;
	pid_t pid;
	FILE *file[2];
	int oldFD[2];
	int fileFD[2];
	char **envp;
	std::string body;
	Config config;
	initEnvp(request);

	body = request.body;
	try
	{
		envp = this->ENVPChangeStringArray();
	}
	catch (std::bad_alloc &e)
	{
		std::cerr << e.what() << std::endl;
	}
	// 입력과 출력 스트림의 사본을 생성하고 저장
	oldFD[0] = dup(STDIN_FILENO);
	oldFD[1] = dup(STDOUT_FILENO);
	if (oldFD[0] == -1 || oldFD[1] == -1)
		throw std::runtime_error("Error saving file descriptor");
	// 파일 데이터 스트림을 생성
	file[0] = tmpfile();
	file[1] = tmpfile();
	if (!file[0] || !file[1])
		throw std::runtime_error("Error creating file for temporary work");

	fileFD[0] = fileno(file[0]);
	fileFD[1] = fileno(file[1]);
	write(fileFD[0], body.c_str(), body.size());
	if (fileFD[0] == -1 || fileFD[1] == -1)
		throw std::runtime_error("Error creating file descriptor");
	write(fileFD[0], body.c_str(), body.size());
	lseek(fileFD[0], 0, SEEK_SET);

	std::cout << "cgi - body : " << body << std::endl;

	pid = fork();
	if (pid == -1)
		throw std::runtime_error("Error create child process");
	else if (pid == 0)
	{
		isCgiPath();
		dup2(fileFD[0], STDIN_FILENO);
		dup2(fileFD[1], STDOUT_FILENO);
		if (execve(context.c_str(), NULL, envp) == -1)
		{
			std::cerr << "Error execute child process: " << strerror(errno) << std::endl;
			throw std::runtime_error("Error execute child process");
		}
		std::cerr << "Error status: 500" << std::endl;
	}
	else
	{
		// wait for child process to finish
		char buffer[100001];
		if (waitpid(pid, NULL, 0) == -1)
			throw std::runtime_error("Error waiting for child process");
		lseek(fileFD[1], 0, SEEK_SET);
		// read output from the file descriptor and print it to console
		int bytes = 1;
		body = "";
		while (bytes > 0)
		{
			memset(buffer, 0, 100001);
			bytes = read(fileFD[1], buffer, 100000);
			body += buffer;
		}
	}

	setBody(body);
	dup2(oldFD[0], 0);
	dup2(oldFD[1], 1);

	fclose(file[0]);
	fclose(file[1]);

	close(fileFD[0]);
	close(fileFD[1]);
	close(oldFD[0]);
	close(oldFD[1]);
	// delete envp
	for (int i = 0; envp[i]; ++i)
	{
		delete[] envp[i];
	}
	delete[] envp;
	if (pid == 0)
		exit(0);
	std::cout << "cgi body size : " << body.length() << std::endl;
	return (body);
}
