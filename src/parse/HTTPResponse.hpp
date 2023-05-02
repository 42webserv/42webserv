/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 13:33:00 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/02 14:44:13 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include <iostream>
#include <string>
#include <unistd.h>
#include "HTTPRequestParser.hpp"

std::string generateErrorHeader(int status_code, const std::string &message);
std::string generateHeader(const std::string &content);
std::string getResource(const std::string &path);
void requestHandler(const HTTPRequest &request, int client_fd);

#endif
