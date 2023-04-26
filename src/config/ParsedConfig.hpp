/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/04/20 21:31:04 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSED_CONFIG_HPP
#define PARSED_CONFIG_HPP

#include "Config.hpp"

class ParsedConfig : public Config
{
private:
public:
    ParsedConfig(/* args */);
    ~ParsedConfig();
};

ParsedConfig::ParsedConfig(/* args */)
{
}

ParsedConfig::~ParsedConfig()
{
}

#endif