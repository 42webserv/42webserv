/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 17:22:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 15:00:09 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "Directive.hpp"

enum ConfigType
{
	SIMPLE,
	BLOCK
};

class Config
{
private:
	Directive parseDirective(const std::string &line);
	void setBlock(std::ifstream &infile, std::vector<Directive> &directive);

protected:
	std::vector<Directive> _directives;

public:
	Config();
	~Config();
	void loadFromFile(const std::string &filename);
	void printConfig(std::vector<Directive> directives, size_t tab);
	const std::vector<Directive> getDirectives() const;
};

#endif
