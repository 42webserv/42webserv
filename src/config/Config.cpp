/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:55:04 by seokchoi          #+#    #+#             */
/*   Updated: 2023/06/15 13:14:16 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <sys/stat.h>
#include "CheckConfigValid.hpp"
#include "Utils.hpp"
#include "commonConfig.hpp"
#include "Config.hpp"
#include "DefaultConfig.hpp"

Config::Config()
{
	this->_setRelation();
}

Config::~Config()
{
	// TODO clearBlock을 만들고 재귀로 돌면서 가장 안쪽 블록부터 지워나가기
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
		directive.name = DEFALUT_DIRECTIVE_VALUE_FAIL;
		return directive;
	}
	trimdLine = Config::trim(line);
	size_t pos = trimdLine.find(' '); // 첫 번째 공백의 위치를 찾는다.
	if (pos == std::string::npos)	  // 공백이 없는 경우
	{
		directive.name = DEFALUT_DIRECTIVE_VALUE_FAIL; // 디렉티브의 이름을 저장
		return directive;
	}

	directive.name = trimdLine.substr(0, pos);					 // 디렉티브의 이름을 저장
	size_t value_pos = trimdLine.find_first_not_of(SP, pos + 1); // 공백이 아닌 문자를 찾는다.
	directive.value = trimdLine.substr(value_pos, trimdLine.size() - value_pos);
	if (directive.value[directive.value.length() - 1] == ';')
		directive.value = trimdLine.substr(value_pos, trimdLine.size() - value_pos - 1);
	if (directive.value.find(LEFT_BRACE) != std::string::npos)
	{
		directive.value.erase(directive.value.find(LEFT_BRACE), 1);
		directive.value = Config::trim(directive.value);
	}
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
		if (line.find(RIGHT_BRACE) != std::string::npos)
			return;
		Directive directive = this->_parseDirective(line);
		if (directive.name == DEFALUT_DIRECTIVE_VALUE_FAIL)
			continue;
		directive.pre_name = pre_name;
		directives.push_back(directive);
		if (line.find(LEFT_BRACE) != std::string::npos)
		{
			_setBlock(infile, directives.back().block, directives.back().name);
		}
	}
}

void Config::_setRelation()
{
	// main
	_main.insert(std::make_pair(TYPES_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_main.insert(std::make_pair(HTTP_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));

	// http
	_http.insert(std::make_pair(INCLUDE_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_http.insert(std::make_pair(INDEX_DIRECTIVE, DEFAULT_SERVER_INDEX));
	_http.insert(std::make_pair(SERVER_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_http.insert(std::make_pair(CLIENT_MAX_BODY_SIZE_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));

	// server
	_server.insert(std::make_pair(LISTEN_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(SERVER_NAME_DIRECTIVE, "nobody"));
	_server.insert(std::make_pair(ERROR_PAGE_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(CLIENT_MAX_BODY_SIZE_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(ROOT_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(LOCATION_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(INDEX_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_server.insert(std::make_pair(LIMIT_EXCEPT_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));

	// location
	_location.insert(std::make_pair(ROOT_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(INDEX_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(CLIENT_MAX_BODY_SIZE_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(AUTOINDEX_DIRECTIVE, "off"));
	_location.insert(std::make_pair(LIMIT_EXCEPT_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(RETURN_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(CGI_EXTENSION_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
	_location.insert(std::make_pair(CGI_PATH_DIRECTIVE, DEFALUT_DIRECTIVE_VALUE_FAIL));
};

void Config::_setIncludes()
{
	std::vector<Directive> includes;
	this->getAllDirectives(includes, _directives, INCLUDE_DIRECTIVE);
	for (size_t i = 0; i < includes.size(); i++)
	{
		std::ifstream includeFile;
		includeFile.open(includes[i].value);
		if (!includeFile.is_open())
			stderrExit("Error: Invalid include config file '" + includes[i].value + "'");
		std::vector<Directive> includeDirectives;
		_setBlock(includeFile, includeDirectives, MAIN_DIRECTIVE);
		_directives[0].block.push_back(includeDirectives[0]);
		includeFile.close();
	}
}

void Config::parsedConfig(int argc, char const **argv)
{
	std::string filename;
	std::ifstream infile;

	if (argc != 1 && argc != 2)
		stderrExit("Usage: ./webserv [config_file]");
	if (argc == 2)
	{
		if (!CheckConfigValid::Parse(argv[1]))
			stderrExit("Error: Invalid config file");
		filename = argv[1];
	}
	else
	{
		if (!CheckConfigValid::Parse(DEFAULT_CONF_PATH))
			stderrExit("Error: Invalid config file");
		filename = DEFAULT_CONF_PATH;
	}
	infile.open(filename);
	if (!infile.is_open())
		stderrExit("Error: Invalid config file '" + filename + "'");
	_setBlock(infile, _directives, MAIN_DIRECTIVE);
	if (_directives.size() == 0) // empty file
		stderrExit("Error: Invalid config file '" + filename + "'");
	if (_directives[0].name != MAIN_DIRECTIVE)
	{
		std::vector<Directive> tmp = _directives;
		_directives[0].name = MAIN_DIRECTIVE;
		_directives[0].block = tmp;
	}
	_checkRealtion(_directives[0].block);
	_setIncludes();
	infile.close();

	// Check configuration file
	DefaultConfig defaultConfig(this->_directives);
	defaultConfig.checkDirectives();
}

void Config::printDirectives(std::vector<Directive> directive, size_t tab)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		for (size_t j = 0; j < tab; j++)
		{
			std::cout << "\t";
		}
		// std::cout << "  " << directive[i].name << " : " << directive[i].value << " : " << directive[i].pre_name << std::endl;
		if (directive[i].block.empty())
			continue;
		Config::printDirectives(directive[i].block, tab + 1);
	}
}

const std::vector<Directive> &Config::getDirectives() const
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
void Config::getAllDirectives(std::vector<Directive> &newDirectives, const std::vector<Directive> &directives, const std::string dirName) const
{
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (directives[i].name == dirName)
			newDirectives.push_back(directives[i]);
		if (directives[i].block.empty())
			continue;
		this->getAllDirectives(newDirectives, directives[i].block, dirName);
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
void Config::_checkParent(std::string &parentName, std::string rightPre, std::string blockName) const
{
	if (parentName != rightPre)
		stderrExit("Error: " + blockName + " directive must be in " + rightPre + " block");
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
			stderrExit("Error: " + block[j].name + " directive can not be in " + prarentBlockName + " block");
	}
}

/*
 * 중복되면 안되는 지시자들 확인
 */
void Config::_checkRepeatition(std::vector<Directive> &directives, std::string &parentName)
{
	for (size_t i = 0; i < directives.size() - 1; i++)
	{
		for (size_t k = i + 1; k < directives.size(); k++)
		{
			if (directives[i].name == LISTEN_DIRECTIVE)
				continue;
			if (directives[i].name == SERVER_DIRECTIVE)
				continue;
			if (directives[i].name == LOCATION_DIRECTIVE)
				continue;
			if (directives[i].name == ERROR_PAGE_DIRECTIVE)
				continue;
			if (directives[i].name == directives[k].name)
				stderrExit("Error: The same " + directives[i].name + " directive exists within " + parentName + " directive.");
		}
	}
}

std::vector<std::string> Config::split(std::string input, char delimiter)
{
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter))
	{
		answer.push_back(temp);
	}

	return answer;
}

void Config::_checkEmpty(std::string &value, std::string directiveName, bool exist)
{
	if (value.empty() && exist)
		stderrExit("Error: " + directiveName + " value must be not empty");
	if (!value.empty() && !exist)
		stderrExit("Error: " + directiveName + " value must be empty");
}

// 필요없을 시 지울 것 // _isFileExists, _isDirectoryExists을 위해서 남겨놓음.
// void Config::_checkValidValue(std::vector<Directive> &directives, std::vector<Directive> &preDirective)
void Config::_checkValidValue(std::vector<Directive> &directives)
{
	std::string defaultRoot = "";
	for (size_t i = 0; i < directives.size(); i++)
	{
		if (directives[i].name == HTTP_DIRECTIVE)
			_checkEmpty(directives[i].value, HTTP_DIRECTIVE, false);
		if (directives[i].name == TYPES_DIRECTIVE)
			_checkEmpty(directives[i].value, TYPES_DIRECTIVE, false);
		if (directives[i].name == SERVER_DIRECTIVE)
			_checkEmpty(directives[i].value, SERVER_DIRECTIVE, false);
		if (directives[i].name == INCLUDE_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, INCLUDE_DIRECTIVE, true);
			// 필요없을 시 지울 것 //
			// _isFileExists(directives, directives[i].value, INCLUDE_DIRECTIVE, preDirective);
		}
		if (directives[i].name == SERVER_NAME_DIRECTIVE)
			_checkEmpty(directives[i].value, SERVER_NAME_DIRECTIVE, true);
		if (directives[i].name == LISTEN_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, LISTEN_DIRECTIVE, true);
			for (size_t j = 0; j < directives[i].value.size(); j++)
			{
				if (directives[i].value[j] < '0' || directives[i].value[j] > '9')
					stderrExit("Error: listen value must be number");
			}
		}
		if (directives[i].name == LOCATION_DIRECTIVE)
			_checkEmpty(directives[i].value, LOCATION_DIRECTIVE, true);
		if (directives[i].name == ROOT_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, ROOT_DIRECTIVE, true);
			// 필요없을 시 지울 것 //
			// _isDirectoryExists(directives[i].value, ROOT_DIRECTIVE);
		}
		if (directives[i].name == ERROR_PAGE_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, ERROR_PAGE_DIRECTIVE, true);
			std::vector<std::string> errorPage = split(directives[i].value, ' ');
			if (errorPage.size() < 2) // 여러개 들어와도 처리하도록 해놔야한다.
				stderrExit("Error: error_page value must be number or file");
			for (size_t j = 0; j < errorPage.size() - 1; j++)
			{
				for (size_t k = 0; k < errorPage[j].length(); k++)
				{
					if (errorPage[j][k] < '0' || errorPage[j][k] > '9')
						stderrExit("Error: error_page errorCode must be number");
				}
			}
			// 필요없을 시 지울 것 //
			// _isFileExists(directives, errorPage[errorPage.size() - 1], ERROR_PAGE_DIRECTIVE, preDirective);
		}
		if (directives[i].name == CLIENT_MAX_BODY_SIZE_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, CLIENT_MAX_BODY_SIZE_DIRECTIVE, true);
			for (size_t j = 0; j < directives[i].value.size(); j++)
			{
				if (directives[i].value[j] < '0' || directives[i].value[j] > '9')
					stderrExit("Error: client_max_body_size value must be number");
			}
		}
		if (directives[i].name == AUTOINDEX_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, AUTOINDEX_DIRECTIVE, true);
			if (directives[i].value != "on" && directives[i].value != "off")
				stderrExit("Error: autoindex value must be on or off");
		}
		if (directives[i].name == INDEX_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, INDEX_DIRECTIVE, true);
			// 필요없을 시 지울 것 //
			// _isFileExists(directives, directives[i].value, INDEX_DIRECTIVE, preDirective);
		}
		if (directives[i].name == LIMIT_EXCEPT_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, LIMIT_EXCEPT_DIRECTIVE, true);
			std::vector<std::string> excepts = split(directives[i].value, ' ');
			for (size_t i = 0; i < excepts.size(); i++)
			{
				// TODO : GET, POST, DELETE, PUT 외에 다른 메소드가 들어오면 에러 처리를 잡아주는 함수로 변경하기.
				if (excepts[i] != GET && excepts[i] != POST && excepts[i] != DELETE && excepts[i] != PUT)
					stderrExit("Error: limit_except value must be GET or POST or DELETE");
			}
		}
		if (directives[i].name == RETURN_DIRECTIVE)
		{
			_checkEmpty(directives[i].value, RETURN_DIRECTIVE, true);
			std::vector<std::string> returns = split(directives[i].value, ' ');
			if (returns.size() != 2)
				stderrExit("Error: return value must be number or file");
			for (size_t j = 0; j < returns[0].size(); j++)
			{
				if (returns[0][j] < '0' || returns[0][j] > '9')
					stderrExit("Error: return errorCode must be number");
			}
		}
		if (directives[i].block.empty())
			continue;
	}
}

/*
 *	Block의 모든 에러 체크 함수
 *
 *	directive : 확인할 지시자들
 */
void Config::_checkRealtion(std::vector<Directive> &directive)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		if (directive[i].name == MAIN_DIRECTIVE)
		{
			_checkChildes(directive[i].block, _main, MAIN_DIRECTIVE);
		}
		if (directive[i].name == HTTP_DIRECTIVE)
		{
			_checkParent(directive[i].pre_name, MAIN_DIRECTIVE, HTTP_DIRECTIVE);
			_checkChildes(directive[i].block, _http, HTTP_DIRECTIVE);
		}
		if (directive[i].name == SERVER_DIRECTIVE)
		{
			_checkParent(directive[i].pre_name, HTTP_DIRECTIVE, SERVER_DIRECTIVE);
			_checkChildes(directive[i].block, _server, SERVER_DIRECTIVE);
		}
		if (directive[i].name == LOCATION_DIRECTIVE)
		{
			_checkParent(directive[i].pre_name, SERVER_DIRECTIVE, LOCATION_DIRECTIVE);
			_checkChildes(directive[i].block, _location, LOCATION_DIRECTIVE);
		}
		if (directive[i].block.empty())
			continue;
		else
			_checkRepeatition(directive[i].block, directive[i].name);
		_checkRealtion(directive[i].block);
	}
	_checkValidValue(directive);
}
