/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/27 14:04:20 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSED_CONFIG_HPP
#define PARSED_CONFIG_HPP

#include "Config.hpp"

class ParsedConfig : public Config
{
private:
public:
    ParsedConfig(const std::string &filename);
    ~ParsedConfig();
};

ParsedConfig::ParsedConfig(const std::string &filename)
{
    
}

ParsedConfig::~ParsedConfig()
{

}

#endif