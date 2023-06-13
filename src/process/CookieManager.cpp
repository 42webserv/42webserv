/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 16:52:35 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 18:06:43 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CookieManager.hpp"

CookieManager::CookieManager(const std::vector<Cookie> &cookies) : cookies(cookies) {}

CookieManager::~CookieManager() {}
