/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:14:03 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/21 19:15:21 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULT_CONFIG_HPP
#define DEFAULT_CONFIG_HPP

#include "Config.hpp"

class DefaultConfig
{
private:
    Config &config;
    void addAndCheckChildDirectives(Directive &dir, std::vector<Directive> &dirs, const std::string &name, void (DefaultConfig::*fn_addDirs)(Directive &dir, const std::string name), void (DefaultConfig::*fn_checkDirs)(std::vector<Directive> &dirs, const std::string pre_name));
    void checkMainDirectives(std::vector<Directive> &dirs);
    void checkHttpDirectives(std::vector<Directive> &dirs, const std::string pre_name);
    void checkServerDirectives(std::vector<Directive> &dirs, const std::string pre_name);
    void checkLocationDirectives(std::vector<Directive> &dirs, const std::string pre_name);
    void addDirectives(std::map<std::string, std::string> &dirs, Directive &dir, const std::string name);
    void addMainDirectives(Directive &main, const std::string name);
    void addHttpDirectives(Directive &http, const std::string name);
    void addServerDirectives(Directive &server, const std::string name);
    void addLocationDirectives(Directive &location, const std::string name);

public:
    DefaultConfig(Config &config);
    ~DefaultConfig();
    void checkDirectives();
};

#endif
