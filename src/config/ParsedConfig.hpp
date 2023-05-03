/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 19:54:32 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSED_CONFIG_HPP
#define PARSED_CONFIG_HPP

#include <string>
#include <vector>
#include "Directive.hpp"

class ParsedConfig
{
private:
    Directive parseDirective(const std::string &line);
    void setBlock(std::ifstream &infile, std::vector<Directive> &directive);
    std::string Trim(const std::string &str);

protected:
    std::vector<Directive> _directives;

public:
    ParsedConfig();
    ~ParsedConfig();
    void parsedConfig(int argc, char const **argv);
    void printDirectives(std::vector<Directive> directives, size_t tab);
    void getAllDirectives(std::vector<Directive> &newDirectives, std::vector<Directive> directives, std::string dirName);
    const std::vector<Directive> getDirectives() const;
};

#endif
