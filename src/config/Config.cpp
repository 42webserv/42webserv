/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:55:04 by seokchoi          #+#    #+#             */
/*   Updated: 2023/05/12 14:24:34 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <stack>
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include <exception>

Config::Config()
{
	this->_directives = std::vector<Directive>();
	this->_setRelation();
}

Config::~Config()
{
	for (size_t i = 0; i < this->_directives.size(); i++)
		this->_directives[i].block.clear();
	this->_directives.clear();
}

Directive Config::_parseDirective(const std::string &line)
{
	Directive directive; // 디렉티브 객체
	std::string trimdLine;
	if (line.empty() || line[0] == '#') // 주석이거나 빈 줄인 경우
	{
		directive.name = "fail";
		return directive;
	}
	trimdLine = Config::trim(line);
	size_t pos = trimdLine.find(' '); // 첫 번째 공백의 위치를 찾는다.
	if (pos == std::string::npos)	  // 공백이 없는 경우
	{
		directive.name = "fail"; // 디렉티브의 이름을 저장
		return directive;
	}

	directive.name = trimdLine.substr(0, pos);					  // 디렉티브의 이름을 저장
	size_t value_pos = trimdLine.find_first_not_of(" ", pos + 1); // 공백이 아닌 문자를 찾는다.
	directive.value = trimdLine.substr(value_pos, trimdLine.size() - value_pos);
	if (directive.value[directive.value.length() - 1] == ';')
		directive.value = trimdLine.substr(value_pos, trimdLine.size() - value_pos - 1);
	if (directive.value.find("{") != std::string::npos)
		directive.value.erase(directive.value.find("{"), 1);
	return directive; // 블록이 있는 경우 블록이 존재하는 블록이 반환된다.
}

void Config::_setBlock(std::ifstream &infile, std::vector<Directive> &directives, std::string pre_name)
{
	std::string line; // 한 줄씩 읽어올 문자열
	size_t pos;		  // 문자열에서 위치를 나타내는 변수
	std::stack<int> blockCheck;

	while (std::getline(infile, line)) // 한 줄씩 읽어오기
	{
		pos = line.find('#', 1);
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		if (line.find("}") != std::string::npos)
			return;
		Directive directive = this->_parseDirective(line);
		if (directive.name == "fail")
			continue;
		directive.pre_name = pre_name;
		directives.push_back(directive);
		if (line.find("{") != std::string::npos)
		{
			_setBlock(infile, directives.back().block, directives.back().name);
		}
	}
}

void Config::_setRelation()
{
	// main
	_main.insert(std::make_pair("types", "fail"));
	_main.insert(std::make_pair("http", "fail"));

	// http
	_http.insert(std::make_pair("include", "fail"));
	_http.insert(std::make_pair("index", "index.html"));
	_http.insert(std::make_pair("server", "fail"));
	_http.insert(std::make_pair("client_max_body_size", "fail"));

	// server
	_server.insert(std::make_pair("listen", "fail"));
	_server.insert(std::make_pair("server_name", "nobody"));
	_server.insert(std::make_pair("error_page", "fail"));
	_server.insert(std::make_pair("client_max_body_size", "fail"));
	_server.insert(std::make_pair("root", "fail"));
	_server.insert(std::make_pair("location", "fail"));

	// location
	_location.insert(std::make_pair("root", "fail"));
	_location.insert(std::make_pair("index", "fail"));
	_location.insert(std::make_pair("client_max_body_size", "fail"));
	_location.insert(std::make_pair("autoindex", "off"));
	_location.insert(std::make_pair("limit_except", "fail"));
	_location.insert(std::make_pair("return", "fail"));
	_location.insert(std::make_pair("cgi_info", "fail"));
};

void Config::_setIncludes()
{
	std::vector<Directive> includes;
	this->getAllDirectives(includes, _directives, "include");
	for (size_t i = 0; i < includes.size(); i++)
	{
		std::ifstream includeFile;
		includeFile.open(includes[i].value);
		if (!includeFile.is_open())
		{
			std::cerr << "Error: Invalid include config file '" << includes[i].value << "'" << std::endl;
			exit(1);
		}
		std::vector<Directive> includeDirectives;
		_setBlock(includeFile, includeDirectives, "main");
		_directives[0].block.push_back(includeDirectives[0]);
		includeFile.close();
	}
}

void Config::parsedConfig(int argc, char const **argv)
{
	std::string filename;
	std::ifstream infile; // 파일 스트림

	if (argc != 1 && argc != 2)
	{
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		exit(1);
	}
	if (argc == 2)
	{
		if (!CheckConfigValid::Parse(argv[1]))
		{
			std::cerr << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = argv[1];
	}
	else
	{
		if (!CheckConfigValid::Parse("src/config/default.conf"))
		{
			std::cerr << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = "src/config/default.conf";
	}
	infile.open(filename);
	_setBlock(infile, _directives, "main");
	if (_directives[0].name != "main")
	{
		std::vector<Directive> tmp = _directives;
		_directives[0].name = "main";
		_directives[0].block = tmp;
	}
	_checkRealtion(_directives);
	_setIncludes();
	infile.close();
}

void Config::printDirectives(std::vector<Directive> directive, size_t tab)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		for (size_t j = 0; j < tab; j++)
		{
			std::cout << "\t";
		}
		std::cout << "  " << directive[i].name << " : " << directive[i].value << " : " << directive[i].pre_name << std::endl;
		if (directive[i].block.empty())
			continue;
		Config::printDirectives(directive[i].block, tab + 1);
	}
}

const std::vector<Directive> Config::getDirectives() const
{
	return this->_directives;
}

/*
 *	원하는 directive name을 가진 모든 지시자들을 찾아주는 함수
 *
 *	newDirectives : 담아줄 Directive vector
 *	directives : 찾을 Directive vector
 *	dirName :	찾을 Directive의 name
 */
void Config::getAllDirectives(std::vector<Directive> &newDirectives, std::vector<Directive> directives, std::string dirName)
{
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (directives[i].name == dirName)
		{
			newDirectives.push_back(directives[i]);
		}
		if (directives[i].block.empty())
			continue;
		Config::getAllDirectives(newDirectives, directives[i].block, dirName);
	}
}

std::string Config::trim(const std::string &str)
{
	std::size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
	{
		return "";
	}
	std::size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/*
 * 지시자들의 상관 관계를 확인하는 함수
 *
 */
void Config::_checkParent(std::string parentName, std::string rightPre, std::string blockName)
{
	if (parentName != rightPre)
	{
		std::cerr << "Error: " << blockName << " directive must be in " << rightPre << " block" << std::endl;
		exit(1);
	}
}

/*
 * 자식이 될 수 있는 지사자들을 체크한다.
 */
void Config::_checkChildes(std::vector<Directive> &block, std::map<std::string, std::string> &blockFormat, std::string prarentBlockName)
{
	std::map<std::string, std::string>::iterator it;

	for (size_t j = 0; j < block.size(); j++)
	{
		it = blockFormat.find(block[j].name);
		if (it == blockFormat.end())
		{
			std::cerr << "Error: " << block[j].name << " directive can not be in "
					  << prarentBlockName
					  << " block" << std::endl;
			exit(1);
		}
	}
}

/*
 *	Block의 모든 에러 체크 함수
 *
 *	directive : 확인할 지시자들
 */
void Config::_checkRealtion(std::vector<Directive> directive)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		if (directive[i].name == "main")
		{
			_checkChildes(directive[i].block, _main, "main");
		}
		if (directive[i].name == "http")
		{
			_checkParent(directive[i].pre_name, "main", "http");
			_checkChildes(directive[i].block, _http, "http");
		}
		if (directive[i].name == "server")
		{
			_checkParent(directive[i].pre_name, "http", "server");
			_checkChildes(directive[i].block, _server, "server");
		}
		if (directive[i].name == "location")
		{
			_checkParent(directive[i].pre_name, "server", "location");
			_checkChildes(directive[i].block, _location, "location");
		}
		if (directive[i].block.empty())
			continue;
		Config::_checkRealtion(directive[i].block);
	}
}