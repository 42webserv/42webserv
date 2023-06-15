/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:29:58 by yje               #+#    #+#             */
/*   Updated: 2023/06/12 18:13:18 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

// CGI 환경변수 세팅
CGI::CGI(const HTTPRequest &request) : body_(request.body)
{
	initEnvp(request);
}

CGI::~CGI() {}

void CGI::initEnvp(const HTTPRequest &request) // request config 이름 확인해서 받아오기
{
	if (Utils::ftToString(request.body.length()) == "0")
		this->envp_["CONTENT_LENGTH"] = "-1";
	else
		this->envp_["CONTENT_LENGTH"] = Utils::ftToString(request.body.length());
	this->envp_["CONTENT_TYPE"] = Utils::getContentType(request);
	this->envp_["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->envp_["PATH_INFO"] = request.path;
	// PATH_INFO의 변환. 스크립트의 가상경로를, 실제 호출 할 때 사용되는 경로로 맵핑.
	//  요청 URI의 PATH_INFO 구성요소를 가져와, 적합한 가상 : 실제 변환을 수행하여 맵핑.
	this->envp_["QUERY_STRING"] = request.query;
	this->envp_["REMOTE_ADDR"] = request.addr;
	this->envp_["REQUEST_METHOD"] = request.method;
	this->envp_["REQUEST_URI"] = request.path;	//
	this->envp_["SCRIPT_NAME"] = "webserv/1.1"; //
	this->envp_["SERVER_PORT"] = Utils::ftToString(request.port); // 요청을 수신한 서버의 포트 번호.
	this->envp_["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->envp_["SERVER_SOFTWARE"] = "webserv/1.1";
};

void CGI::setEnvp(std::string key, std::string value)
{
	this->envp_[key] = value;
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
 */
std::string CGI::executeCGI(const std::string& program, ResponseData *response)
{
    char** envp;
    FILE* files[2];
    int fileFds[2];
    int stdFds[2] = { dup(STDIN_FILENO), dup(STDOUT_FILENO) };
    std::string body;

    try
    {
        envp = this->ENVPChangeStringArray();
    }
    catch (std::bad_alloc& e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // 입력과 출력 스트림의 사본을 생성하고 저장
    stdFds[R] = dup(STDIN_FILENO);
    stdFds[W] = dup(STDOUT_FILENO);
    if (stdFds[R] == -1 || stdFds[W] == -1)
        throw std::runtime_error("Error saving file descriptor");

    // 파일 데이터 스트림을 생성
    files[R] = tmpfile();
    files[W] = tmpfile();
    if (!files[0] || !files[1])
        throw std::runtime_error("Error creating file for temporary work");

    fileFds[R] = fileno(files[R]);
    fileFds[W] = fileno(files[W]);
    if (write(fileFds[R], body_.c_str(), body_.size()) < 0)
        throw std::runtime_error("Error writing to file");
    lseek(fileFds[R], 0, SEEK_SET);

    const pid_t pid = fork();
    if (pid == -1) // Error
        throw std::runtime_error("Error creating child process");
    else if (pid == 0) // Child process
        childProcess(fileFds, program, envp);
    else if (0 < pid) // Parent process
		parentProcess(pid, fileFds, body, response);
	dup2(stdFds[R], STDIN_FILENO), dup2(stdFds[W], STDOUT_FILENO);
	close(stdFds[R]), close(stdFds[W]);

	fclose(files[R]), fclose(files[W]);
	close(fileFds[R]), close(fileFds[W]);

	// delete envp
	for (int i = 0; envp[i]; ++i)
		delete[] envp[i];
	delete[] envp;

	if (pid == 0)
		exit(EXIT_SUCCESS);
	return (body);
}



// std::string CGI::executeCGI(const std::string &program)
// {
// 	char **envp;
// 	FILE *files[2];
// 	int fileFds[2];
// 	int stdFds[2] = {dup(STDIN_FILENO), dup(STDOUT_FILENO)};
// 	std::string body;

// 	try
// 	{
// 		envp = this->ENVPChangeStringArray();
// 	}
// 	catch (std::bad_alloc &e)
// 	{
// 		std::cerr << e.what() << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	// 입력과 출력 스트림의 사본을 생성하고 저장
// 	stdFds[R] = dup(STDIN_FILENO);
// 	stdFds[W] = dup(STDOUT_FILENO);
// 	if (stdFds[R] == -1 || stdFds[W] == -1)
// 		throw std::runtime_error("Error saving file descriptor");
// 	// 파일 데이터 스트림을 생성
// 	files[R] = tmpfile();
// 	files[W] = tmpfile();
// 	if (!files[0] || !files[1])
// 		throw std::runtime_error("Error creating file for temporary work");

// 	fileFds[R] = fileno(files[R]);
// 	fileFds[W] = fileno(files[W]);
// 	if (write(fileFds[R], body_.c_str(), body_.size()) < 0)
// 		throw(std::runtime_error("Error writing to file"));
// 	// write(fileFds[R], resource_.c_str(), resource_.size());
// 	if (fileFds[R] == -1 || fileFds[W] == -1)
// 		throw std::runtime_error("Error creating file descriptor");
// 	lseek(fileFds[R], 0, SEEK_SET);
// 	time_t startTime = time(NULL);
// 	const pid_t pid = fork();
// 	if (pid == -1) // Error
// 		throw std::runtime_error("Error create child process");
// 	else if (pid == 0) // Child process
// 		childProcess(fileFds, program, envp);
// 	else if (0 < pid) // Parent process
// 		parentProcess(pid, fileFds, body);

// 	dup2(stdFds[R], STDIN_FILENO), dup2(stdFds[W], STDOUT_FILENO);
// 	close(stdFds[R]), close(stdFds[W]);

// 	fclose(files[R]), fclose(files[W]);
// 	close(fileFds[R]), close(fileFds[W]);

// 	// delete envp
// 	for (int i = 0; envp[i]; ++i)
// 		delete[] envp[i];
// 	delete[] envp;

// 	if (pid == 0)
// 		exit(EXIT_SUCCESS);
// 	return (body);
// }

void CGI::childProcess(const int fileFds[2], const std::string& program, char** envp)
{
    dup2(fileFds[R], STDIN_FILENO);
    dup2(fileFds[W], STDOUT_FILENO);

    // 파이썬 파일 실행
    if (execve(program.c_str(), NULL, envp) == -1)
    {
        // execve() 실행에 실패한 경우 오류 처리
        std::cerr << "Error executing child process: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void CGI::parentProcess(const pid_t &pid, const int fileFds[2], std::string &body,ResponseData *response)
{
	int childStatus;
    if (waitpid(pid, &childStatus, 0) == -1)
        throw std::runtime_error("Error waiting for child process");

    if (WIFEXITED(childStatus))
	{
		int exitStatus = WEXITSTATUS(childStatus);
		if (exitStatus != 0)
		{
			// Child process exited with non-zero status (indicating an error)
			std::cerr << "Child process exited with error status: " << exitStatus << std::endl;
			response->statusCode = 500;
			body = Utils::errorPageGenerator(response, response->statusCode);
			return;
		}
	}
	// else if (WIFSIGNALED(childStatus))
	// {
	// 	int signalNumber = WTERMSIG(childStatus);
	// 	std::cerr << "Child process terminated with signal: " << signalNumber << std::endl;
	// 	response->statusCode = 500;
	// 	body = Utils::errorPageGenerator(response, response->statusCode);
	// 	// return body;
	// 	return;
	// }
	char buffer[100001];
	lseek(fileFds[W], 0, SEEK_SET);
	// read output from the file descriptor and print it to console
	int bytes = 1;
	while (bytes > 0)
	{
		memset(buffer, 0, 100001);
		bytes = read(fileFds[W], buffer, 100000);
		if (bytes < 0)
			throw std::runtime_error("Error reading from file");
		body += buffer;
	}
}
