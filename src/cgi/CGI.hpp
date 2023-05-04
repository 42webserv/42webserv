/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sanghan <sanghan@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 14:59:31 by sanghan           #+#    #+#             */
/*   Updated: 2023/05/04 15:33:36 by sanghan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>

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
