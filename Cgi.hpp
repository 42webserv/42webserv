/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 17:35:20 by yje               #+#    #+#             */
/*   Updated: 2023/04/28 18:23:44 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

class CGI {
	private:
		cgi(void);
	public:
		CGI();
		CGI(const CGI &src);
		CGI &operator=(const CGI &src);
		~CGI(void);
		cgiRequest();
		initEnvp();
		getenv(const char* env);

		std::map<std::string, std::string> _envp;


};
#endif;
