/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:59:10 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/01 17:55:40 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"

Directive *new_dir(const std::string &name, const std::string &value)
{
	Directive *dir = new Directive();

	dir->name = name;
	dir->value = value;

	return dir;
}

DefaultConfig::DefaultConfig()
{
	this->directives.clear();
	Directive *dir = new_dir("user", "nginx");
	this->directives.push_back(*dir);
	delete dir;
}

DefaultConfig::~DefaultConfig()
{
}
