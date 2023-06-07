/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 19:53:58 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 17:04:51 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonProcess.hpp"
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

int ftStoi(const std::string &str)
{
	std::stringstream ss(str);
	int requestBodySize;
	ss >> requestBodySize;

	return requestBodySize;
}

bool needBody(const std::string &method)
{
	if (method.empty())
		return false;
	if (isEqual(method, POST) || isEqual(method, PUT))
		return true;
	return false;
}
