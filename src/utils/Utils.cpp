/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:16:36 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/06 15:30:52 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

/*
 * A default constructor
 */
Utils::Utils()
{
}

/*
 * A destructor
 */
Utils::~Utils()
{
}

size_t Utils::minPos(size_t p1, size_t p2, size_t p3)
{
    return (p1 < p2 && p1 < p3 ? p1 : (p2 < p3 && p2 < p1 ? p2 : p3));
}
