/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:14:03 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 16:20:32 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULT_CONFIG_HPP
#define DEFAULT_CONFIG_HPP

#include "Config.hpp"

class DefaultConfig : public Config
{
private:
    void setDefaults();

public:
    DefaultConfig();
    ~DefaultConfig();
};

#endif
