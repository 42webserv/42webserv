/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 21:38:29 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 17:12:40 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <stack>
#include "Config.hpp"

Config::Config()
{
    this->_directives = std::vector<Directive>();
}

Config::~Config()
{
    for (size_t i = 0; i < this->_directives.size(); i++)
        this->_directives[i].block.clear();
    this->_directives.clear();
}

Directive Config::parseDirective(const std::string &line)
{
    Directive directive; // 디렉티브 객체

    if (line.empty() || line[0] == '#') // 주석이거나 빈 줄인 경우
    {
        directive.name == "fail";
        return directive;
    }
    size_t pos = line.find(' '); // 첫 번째 공백의 위치를 찾는다.
    // 공백을 찾는 이유는? 첫 번째 공백 전까지가 디렉티브의 이름이기 때문이다.
    if (pos == std::string::npos) // 공백이 없는 경우
    {
        directive.name = "fail"; // 디렉티브의 이름을 저장
        return directive;
    }

    directive.name = line.substr(0, pos);                              // 디렉티브의 이름을 저장
    size_t value_pos = line.find_first_not_of(" ", pos + 1);           // 공백이 아닌 문자를 찾는다.
    directive.value = line.substr(value_pos, line.size() - value_pos); // 디렉티브의 값을 저장

    if (line.find("{") != std::string::npos)    // 블록이 있는 경우
        directive.block.push_back(Directive()); // 블록을 추가한다.
    return directive;                           // 블록이 있는 경우 블록이 존재하는 블록이 반환된다.
}

void Config::setBlock(std::ifstream infile, std::vector<Directive> &directives)
{
    std::string line; // 한 줄씩 읽어올 문자열
    size_t pos;       // 문자열에서 위치를 나타내는 변수
    std::stack<int> block_check;

    while (std::getline(infile, line)) // 한 줄씩 읽어오기
    {
        pos = line.find('#', 1);
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        Directive directive = this->parseDirective(line);
        if (directive.name == "fail")
            continue;
        directives.push_back(directive);
        // if (!directive.block.empty())

        // if (!directives.empty())
        //     directives.push_back(directive);
        // else
        //     currentBlock->push_back(*directive);
        // if (!directive->block.empty())
        //     block_stack.push(&directive->block.back());
        if (line.find("{") != std::string::npos)
        {
            // directives.back().block.push_back(Directive());
            block_check.push(1);
            setBlock(infile, directives.back()->block);
            // block_stack.push(&directive->block.back());
        }
        else if (line.find("}") != std::string::npos)
        {
            if (block_check.empty())
                break;
            else
                block_check.pop();
        }
    }
}

void Config::loadFromFile(const std::string &filename)
{
    std::ifstream infile;                // 파일 스트림
    std::stack<Directive *> block_stack; // 블록 스택
    // std::string line;                    // 한 줄씩 읽어올 문자열
    // size_t pos;                          // 문자열에서 위치를 나타내는 변수

    // infile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // 예외 처리
    // 파일을 읽다가 에러가 나는 경우 프로그램이 아예 종료가 될 수 있으니까
    // 예외 처리를 해서 에러를 던져서 예외를 처리할 수 있게 해준다.
    infile.open(filename); // 파일 열기

    setBlock(infile, _directives);
    // while (std::getline(infile, line)) // 한 줄씩 읽어오기
    // {
    //     pos = line.find('#', 1);                           // 뒤에 붙은 주석 찾기
    //     if (pos != std::string::npos)                      // 주석인 경우,
    //         line = line.substr(0, pos);                    // 시작부터 주석까지 부분을 line에 넣는다.
    //     Directive *directive = this->parseDirective(line); // 디렉티브 파싱
    //     if (directive == NULL)                             // 주석이거나 빈 줄인 경우
    //         continue;
    //     if (!block_stack.empty())                           // 블록이 있는 경우, block_stack에 block이 있다는 것은
    //                                                         // 스택의 top을 하기 전에 이렇게 먼저 비어있지는 않은지 검사를 해야한다.
    //         block_stack.top()->block.push_back(*directive); //  block이 존재한다는 것이다.
    //     else                                                // 블록이 없는 경우
    //         _directives.push_back(*directive);              // block_stack이 비어있는 경우는 첫 블록이라는 뜻이다. or 다른 경우도 있을 수도?
    //     // if (!directive->block.empty())                      // 블록이 있는 경우, 즉 블록인 directive인 경우를 말한다.
    //     //     block_stack.push(&directive->block.back());     // 블록이 있는 경우라면 block_stack에 block을 push한다.
    //     if (line.find("{") != std::string::npos)        // 블록이 시작되는 경우 == directive에 block이 있는 경우.
    //         block_stack.push(&directive->block.back()); // 잠까 근데 위 코드랑 같은 기능하는거 아니야? 왜 두 번 push해?
    //     else if (line.find("}") != std::string::npos)
    //         block_stack.pop();
    // }
    infile.close();
}

/*
    block_stack에는 어떤 형태로 config가 저장 돼?
    block_stack에는 block이 저장돼.
    block이란?
    block은 Directive의 vector이다.
    Directive는 name, value, block으로 이루어져 있다.
    block_stack에는 block이 저장돼 있고, block은 Directive의 vector이다.
*/

void Config::printConfig(std::vector<Directive> directive)
{
    for (size_t i = 0; i < directive.size(); i++)
    {
        std::cout << "  " << directive[i].name << ": " << directive[i].value << std::endl;
        if (directive[i].block.empty())
            continue;
        std::cout << "  block: " << std::endl;
        Config::printConfig(directive[i].block);

        // for (size_t j = 0; j < _directives[i].block.size(); j++)
        // {
        //     std::cout << "    " << _directives[i].block[j].name << ": " << _directives[i].block[j].value << std::endl;
        // }
    }
}

const std::vector<Directive> Config::getDirectives() const
{
    return this->_directives;
}