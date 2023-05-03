/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 15:06:30 by sanghan           #+#    #+#             */
/*   Updated: 2023/05/03 15:51:35 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>
#include <map>
#include <string>

#include "../parse/HTTP/HTTPRequestParser.hpp"
//#include "HTTP/HTTPRequestParser.hpp"

class CGI
{
private:
public:
	CGI(void);
	virtual ~CGI(void);
	CGI(const CGI &obj);
	CGI &operator=(const CGI &obj);
};

#endif
