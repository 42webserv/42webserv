/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:14:03 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/13 20:05:17 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULT_CONFIG_HPP
#define DEFAULT_CONFIG_HPP

#include "Config.hpp"

class DefaultConfig
{
private:
    Config &config;
    void checkMainDirectives(std::vector<Directive> &dirs);
    void checkHttpDirectives(std::vector<Directive> &dirs);
    void checkServerDirectives(std::vector<Directive> &dirs);
    void checkLocationDirectives(std::vector<Directive> &dirs);
    // void addMainDirectives(std::vector<Directive> &main);
    // void addHttpDirectives(std::vector<Directive> &http);
    // void addServerDirectives(std::vector<Directive> &server);
    // void addLocationDirectives(std::vector<Directive> &location);

public:
    DefaultConfig(Config &config);
    ~DefaultConfig();
    void checkDirectives();
};

#endif
