/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/04 16:54:10 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "Directive.hpp"

/*
 * default 값들을 저장하는 구조체
 *
 * 지시자들의 상관 관계를 정리하기 위해서 만든 구조체
 *
 */
typedef struct s_http_default
{
    std::string include;
    std::string index;
    t_server_default server;
} t_http_default;

typedef struct s_server_default
{
    std::string listen;
    std::string server_name;
    std::string error_page;
    std::string client_max_body_size;
    std::string root;
    t_location_default location;
} t_server_default;

typedef struct s_location_default
{
    std::string root;
    std::string index;
    std::string autoindex;
    std::string limit_except;
    std::string return_;
} t_location_default;

class Config
{
private:
    Directive parseDirective(const std::string &line);
    void setBlock(std::ifstream &infile, std::vector<Directive> &directive);
    std::string trim(const std::string &str);

protected:
    std::vector<Directive> _directives;

public:
    Config();
    ~Config();
    void parsedConfig(int argc, char const **argv);
    void printDirectives(std::vector<Directive> directives, size_t tab);
    void getAllDirectives(std::vector<Directive> &newDirectives, std::vector<Directive> directives, std::string dirName);
    const std::vector<Directive> getDirectives() const;
};

#endif
