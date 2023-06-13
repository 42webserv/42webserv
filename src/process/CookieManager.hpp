/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 16:52:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 18:02:28 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_MANAGER_HPP
#define COOKIE_MANAGER_HPP

#include <string>
#include <vector>

struct Cookie
{
	std::string name;
	std::string value;
	Cookie(const std::string &name, const std::string &value) : name(name), value(value) {}
};

class CookieManager
{
private:
	std::vector<Cookie> cookies;

public:
	CookieManager(const std::vector<Cookie> &cookies);
	~CookieManager();
	void addCookie(const std::string &cookie);
};

#endif
