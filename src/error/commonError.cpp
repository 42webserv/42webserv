/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:27:34 by yje               #+#    #+#             */
/*   Updated: 2023/05/25 19:19:00 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "commonError.hpp"

void errorExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void stderrExit(const std::string msg)
{
    std::cerr << msg << '\n';
    exit(EXIT_FAILURE);
}
