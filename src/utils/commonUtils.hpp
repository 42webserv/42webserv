/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/02 19:53:58 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/05 15:20:23 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <sstream>
#include <string>
#include <vector>
#include "Directive.hpp"

std::string lower(const std::string &s);
bool isEqual(const std::string &s1, const std::string &s2);
std::vector<Directive>::const_iterator findDirective(const std::vector<Directive> &directives, const std::string &name);
std::vector<Directive>::const_iterator findDirectiveNameValue(const std::vector<Directive> &directives, const std::string &name, const std::string &value);
int ftStoi(const std::string &str);
template <typename T>
std::string ftToString(T value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
#endif
