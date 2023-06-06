/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 15:16:36 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/06/06 15:47:48 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

/*
 * A destructor
 */
Utils::~Utils() {}

/**
 * 3개의 인자 중 가장 작은 값 반환
 *
 * @param size_t 3개의 양수
 */
size_t Utils::minPos(size_t p1, size_t p2, size_t p3)
{
    return (p1 < p2 && p1 < p3 ? p1 : (p2 < p3 && p2 < p1 ? p2 : p3));
}

/**
 * 10진수 인자를 16진수 문자열로 반환
 *
 * @param size_t 10진수 값
 * @return 16진수 문자열
 */
std::string Utils::toHexString(size_t value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}
