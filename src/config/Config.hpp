/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 17:22:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 13:40:04 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "Directive.hpp"
#include "HTTPRequestParser.hpp"

enum ConfigType
{
	SIMPLE,
	BLOCK
};

class Config
{
private:
	Directive *parseDirective(const std::string &line);

protected:
	std::vector<Directive> directives;

public:
	Config();
	~Config();
	void parsedConfig(const std::string &filename);
	bool valid_request(const HTTPRequest &req);
};

#endif