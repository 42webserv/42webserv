/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 13:33:00 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/04 20:47:53 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include <iostream>
#include <string>
#include <unistd.h>
#include "HTTPRequestParser.hpp"
#include "ParsedConfig.hpp"

std::string generateErrorHeader(int status_code, const std::string &message);
std::string generateHeader(const std::string &content, const std::string &contentType);
void getResponse(const HTTPRequest &request, int client_fd, ParsedConfig &config);
void requestHandler(const HTTPRequest &request, int client_fd, ParsedConfig &config);

#endif
