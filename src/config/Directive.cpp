/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 19:21:54 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:22:20 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include "Directive.hpp"

Directive newDir(const std::string name, const std::string value, const std::string pre_name)
{
	Directive dir = Directive();

	dir.name = name;
	dir.value = value;
	dir.pre_name = pre_name;

	return dir;
}

bool hasDirInBlock(Directive &dir, const std::string name)
{
	for (std::vector<Directive>::iterator it = dir.block.begin(); it != dir.block.end(); it++)
	{
		if ((*it).name == name)
			return true;
	}
	return false;
}

void setDirectivesToMap(std::map<std::string, std::string> &dirs, const std::pair<std::string, std::string> *ps, const size_t count)
{
	for (size_t i = 0; i < count; i++)
		dirs.insert(ps[i]);
}
