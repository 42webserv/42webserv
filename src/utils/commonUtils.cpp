/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 19:53:58 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/05 15:20:17 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonUtils.hpp"

std::string lower(const std::string &s)
{
	std::string lowerS = s;

	for (size_t i = 0; i < lowerS.size(); i++)
	{
		lowerS[i] = std::tolower(lowerS[i]);
	}
	return lowerS;
}

bool isEqual(const std::string &s1, const std::string &s2)
{
	std::string lowerS1 = lower(s1);
	std::string lowerS2 = lower(s2);

	return lowerS1 == lowerS2;
}

std::vector<Directive>::const_iterator findDirective(const std::vector<Directive> &directives, const std::string &name)
{
	for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
	{
		if (isEqual(it->name, name))
			return it;
	}
	return directives.end();
}

std::vector<Directive>::const_iterator findDirectiveNameValue(const std::vector<Directive> &directives, const std::string &name, const std::string &value)
{
	for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); it++)
	{
		if (isEqual(it->name, name) && isEqual(it->value, value))
			return it;
	}
	return directives.end();
}

int ftStoi(const std::string &str)
{
	std::stringstream ss(str);
	int requestBodySize;
	ss >> requestBodySize;

	return requestBodySize;
}
