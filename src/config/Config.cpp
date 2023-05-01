/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 21:38:29 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 14:46:54 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <stack>
#include "Config.hpp"

Config::Config() {}

Config::~Config() {}

Directive *Config::parseDirective(const std::string &line)
{
    Directive *directive;

    if (line.empty() || line[0] == '#')
        return NULL;
    size_t pos = line.find(' ');
    if (pos == std::string::npos)
        return NULL;

    directive = new Directive();
    directive->name = line.substr(0, pos);
    size_t value_pos = line.find_first_not_of(' ', pos + 1);
    directive->value = line.substr(value_pos, line.size() - value_pos);

    if (line.find("{") != std::string::npos)
        directive->block.push_back(Directive());

    return directive;
}

void Config::loadFromFile(const std::string &filename)
{
    std::ifstream infile;
    std::stack<Directive *> block_stack;
    std::string line;
    size_t pos;

    infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    infile.open(filename);

    while (std::getline(infile, line))
    {
        pos = line.find('#', 1);
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        Directive *directive = this->parseDirective(line);
        if (directive == NULL)
            continue;
        if (!block_stack.empty())
            block_stack.top()->block.push_back(*directive);
        else
            directives.push_back(*directive);
        if (!directive->block.empty())
            block_stack.push(&directive->block.back());
        if (line.find("{") != std::string::npos)
            block_stack.push(&directive->block.back());
        else if (line.find("}") != std::string::npos)
            block_stack.pop();
    }
    infile.close();
}

bool Config::valid_request(const HTTPRequest &req)
{
    (void)req;
    return false;
}
