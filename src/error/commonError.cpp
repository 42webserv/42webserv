/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:27:34 by yje               #+#    #+#             */
/*   Updated: 2023/05/30 19:51:20 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "commonError.hpp"

void stderrExit(const std::string msg)
{
    std::cerr << msg << '\n';
    exit(EXIT_FAILURE);
}
