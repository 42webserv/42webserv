/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestProcessor.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 19:12:24 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/13 19:14:17 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PROCESSOR_HPP
#define REQUEST_PROCESSOR_HPP

#include <string>

class RequestProcessor
{
private:
	std::string startLine;
	std::string headers;
	std::string body;

public:
	RequestProcessor();
	~RequestProcessor();
};

#endif
