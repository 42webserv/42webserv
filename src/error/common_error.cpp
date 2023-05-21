/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_error.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:27:34 by yje               #+#    #+#             */
/*   Updated: 2023/05/19 20:39:36 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "common_error.hpp"

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void stderr_exit(const std::string msg)
{
    std::cerr << msg << '\n';
    exit(EXIT_FAILURE);
}
