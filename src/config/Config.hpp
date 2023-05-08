/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/06 23:33:22 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "Directive.hpp"
#include <map>
/*
 * default 값들을 저장하는 구조체
 *
 * 지시자들의 상관 관계를 정리하기 위해서 만든 구조체
 *
 */

typedef struct s_location_default
{
    std::string root;         // 필수
    std::string index;        // 선택
    std::string autoindex;    // 선택
    std::string limit_except; // 선택
    std::string return_;      // 선택
} t_location_default;
typedef struct s_server_default
{
    std::string listen;               // 필수
    std::string server_name;          // 선택 , Default = nobody
    std::string error_page;           // 필수인듯?
    std::string client_max_body_size; // 선택
    std::string root;                 // 필수
    t_location_default location;      // 선택
} t_server_default;

typedef struct s_http_default
{
    std::string include;     // 선택
    std::string index;       // 선택
    t_server_default server; // 필수
} t_http_default;
typedef struct s_main_default
{
    std::string types;   // 선택
    t_http_default http; // 필수
} t_main_default;

class Config
{
private:
    Directive _parseDirective(const std::string &line);
    void _setBlock(std::ifstream &infile, std::vector<Directive> &directive, std::string pre_name);
    std::string trim(const std::string &str);
    std::map<std::string, std::string> _main;
    std::map<std::string, std::string> _http;
    std::map<std::string, std::string> _server;
    std::map<std::string, std::string> _location;
    void _setRelation();
    void _setIncludes();
    void _checkRealtion(std::vector<Directive> directive);

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
