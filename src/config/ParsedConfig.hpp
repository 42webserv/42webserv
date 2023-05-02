/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 20:40:30 by seokchoi         ###   ########.fr       */
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

protected:
    std::vector<Directive> _directives;

public:
    ParsedConfig();
    ~ParsedConfig();
    void loadFromFile(const std::string &filename);
    void printConfig(std::vector<Directive> directives, size_t tab);
    const std::vector<Directive> getDirectives() const;
};

#endif
