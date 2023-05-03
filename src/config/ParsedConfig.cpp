/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/27 13:55:04 by seokchoi          #+#    #+#             */
/*   Updated: 2023/05/03 13:43:59 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <stack>
#include "ParsedConfig.hpp"
#include "CheckConfigValid.hpp"

ParsedConfig::ParsedConfig()
{
	this->_directives = std::vector<Directive>();
}

ParsedConfig::~ParsedConfig()
{
	for (size_t i = 0; i < this->_directives.size(); i++)
		this->_directives[i].block.clear();
	this->_directives.clear();
}

Directive ParsedConfig::parseDirective(const std::string &line)
{
	Directive directive; // 디렉티브 객체

	if (line.empty() || line[0] == '#') // 주석이거나 빈 줄인 경우
	{
		directive.name = "fail";
		return directive;
	}
	size_t pos = line.find(' ');  // 첫 번째 공백의 위치를 찾는다.
	if (pos == std::string::npos) // 공백이 없는 경우
	{
		directive.name = "fail"; // 디렉티브의 이름을 저장
		return directive;
	}

	directive.name = line.substr(0, pos);							   // 디렉티브의 이름을 저장
	size_t value_pos = line.find_first_not_of(" ", pos + 1);		   // 공백이 아닌 문자를 찾는다.
	directive.value = line.substr(value_pos, line.size() - value_pos); // 디렉티브의 값을 저장

	if (directive.value.find("{") != std::string::npos)
		directive.value.erase(directive.value.find("{"), 1);
	return directive; // 블록이 있는 경우 블록이 존재하는 블록이 반환된다.
}

void ParsedConfig::setBlock(std::ifstream &infile, std::vector<Directive> &directives)
{
	std::string line; // 한 줄씩 읽어올 문자열
	size_t pos;		  // 문자열에서 위치를 나타내는 변수
	std::stack<int> blockCheck;

	while (std::getline(infile, line)) // 한 줄씩 읽어오기
	{
		pos = line.find('#', 1);
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		Directive directive = this->parseDirective(line);
		if (directive.name == "fail")
			continue;
		if (line.find("}") != std::string::npos)
		{
			if (blockCheck.empty())
				return;
			else
				blockCheck.pop();
		}
		directives.push_back(directive);
		if (line.find("{") != std::string::npos)
			setBlock(infile, directives.back().block);
	}
}

void ParsedConfig::parsedConfig(int argc, char const **argv)
{
	std::string filename;
	std::ifstream infile; // 파일 스트림

	if (argc != 1 && argc != 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		exit(1);
	}
	if (argc == 2)
	{
		if (!CheckConfigValid::Parse(argv[1]))
		{
			std::cout << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = argv[1];
	}
	else
	{
		if (!CheckConfigValid::Parse("src/config/default.conf"))
		{
			std::cout << "Error: Invalid config file" << std::endl;
			exit(1);
		}
		filename = "src/config/default.conf";
	}

	infile.open(filename);
	setBlock(infile, _directives);
	infile.close();
}

void ParsedConfig::printConfig(std::vector<Directive> directive, size_t tab)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		for (size_t j = 0; j < tab; j++)
		{
			std::cout << "\t";
		}
		std::cout << "  " << directive[i].name << ": " << directive[i].value << std::endl;
		if (directive[i].block.empty())
			continue;
		ParsedConfig::printConfig(directive[i].block, tab + 1);
	}
}

const std::vector<Directive> ParsedConfig::getDirectives() const
{
	return this->_directives;
}
