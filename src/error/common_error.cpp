/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_error.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:56:07 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/24 15:51:45 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "common_error.hpp"

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}