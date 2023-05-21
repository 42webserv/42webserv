/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:15:35 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/21 19:47:49 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <map>
#include <string>
#include <vector>

struct Directive
{
    std::string name;
    std::string value;
    std::string pre_name;
    std::vector<Directive> block;
};

Directive newDir(const std::string name, const std::string value, const std::string pre_name);
bool hasDirInBlock(Directive &dir, const std::string name);
void setDirectivesToMap(std::map<std::string, std::string> &dirs, const std::pair<std::string, std::string> *ps, const size_t count);

#endif
