/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/05/01 17:31:57 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <map>
class CGI
{
	// private:
	//
public:
	CGI();
	CGI(const CGI &src);
	CGI &operator=(const CGI &src);
	~CGI(void);
	// cgiRequest();
	void initEnvp();
	// getenv(const char *env);

	std::map<std::string, std::string> envp_;
};
#endif;
