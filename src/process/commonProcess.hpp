/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonProcess.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:20:30 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/25 19:30:05 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_PROCESS_HPP
#define COMMON_PROCESS_HPP

#include <string>
#include "Response.hpp"

void ftSend(const int &socket, const std::string &buffer);
void ftSend(ResponseData *response, const std::string &response_header);

#endif
