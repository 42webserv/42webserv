/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_error.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:27:34 by yje               #+#    #+#             */
/*   Updated: 2023/05/02 15:27:35 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "common_error.hpp"

void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
