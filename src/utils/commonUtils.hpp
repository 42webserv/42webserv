/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 19:53:58 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 17:11:17 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <sstream>
#include <string>
#include <vector>
#include "Directive.hpp"

int ftStoi(const std::string &str);
template <typename T>
std::string ftToString(T value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
#endif
