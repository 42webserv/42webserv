/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/17 17:22:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/20 21:38:19 by sunhwang         ###   ########.fr       */
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
	Directive *parseDirective(const std::string &line);

protected:
	std::vector<Directive> directives;

public:
	Config();
	~Config();
	void loadFromFile(const std::string &filename);
};

#endif
