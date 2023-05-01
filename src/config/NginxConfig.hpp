/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NginxConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 00:46:56 by seokchoi          #+#    #+#             */
/*   Updated: 2023/05/01 00:50:51 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NGINXCONFIG_HPP
#define NGINXCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Directive.hpp"
#include <fstream>
#include <sstream>

class NginxConfig
{
private:
	std::vector<Directive> directives_;

public:
	NginxConfig() {}
	~NginxConfig() {}

	std::string Trim(const std::string &str);
	bool Parse(const std::string &file_path);
	std::vector<Directive> &GetDirectives();
	void printConfig(std::vector<Directive> directives);
};

#endif