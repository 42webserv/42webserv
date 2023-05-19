/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_directive.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 21:43:08 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/19 21:50:22 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive.hpp"

Directive newDir(const std::string name, const std::string value)
{
	Directive dir = Directive();

	dir.name = name;
	dir.value = value;

	return dir;
}
