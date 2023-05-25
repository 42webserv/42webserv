/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonError.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/24 15:00:20 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:19:06 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_ERROR_HPP
#define COMMON_ERROR_HPP

#include <string>

void errorExit(const char *msg);
void stderrExit(const std::string msg);

#endif
