/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:15:35 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/19 22:03:25 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include <string>
#include <vector>

struct Directive
{
    std::string name;
    std::string value;
    std::string pre_name;
    std::vector<Directive> block;
};

Directive newDir(const std::string name, const std::string value);

#endif
