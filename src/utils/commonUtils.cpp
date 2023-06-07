/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 19:53:58 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 17:11:18 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commonProcess.hpp"
#include "commonUtils.hpp"

int ftStoi(const std::string &str)
{
	std::stringstream ss(str);
	int requestBodySize;
	ss >> requestBodySize;

	return requestBodySize;
}
