/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/05/02 16:32:46 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>
#include <cctype>
class CGI
{
	// private:
	//
public:
	CGI(); //
	CGI(const CGI &src); //
	CGI &operator=(const CGI &src);
	~CGI(void);//
	// cgiRequest();
	void initEnvp(const HTTPRequest& request);
	void CGI::excuteCGI();
	// getenv(const char *env);

	std::map<std::string, std::string> envp_;
};
#endif;
