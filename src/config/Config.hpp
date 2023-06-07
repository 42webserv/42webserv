/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 20:16:55 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/07 09:10:31 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>
#include <vector>
#include "Directive.hpp"
#define DEFAULT_CONF_PATH "assets/conf/default.conf"

/*
 * default 값들을 저장하는 구조체
 *
 * 지시자들의 상관 관계를 정리하기 위해서 만든 구조체
 *
 */
typedef struct s_location_default
{
    std::string root;
    std::string index;
    std::string autoindex;
    std::string limit_except;
    std::string return_;
} t_location_default;

typedef struct s_server_default
{
    std::string listen;
    std::string server_name;
    std::string error_page;
    std::string client_max_body_size;
    std::string root;
    t_location_default location;
} t_server_default;

typedef struct s_http_default
{
    std::string include;
    std::string index;
    t_server_default server;
} t_http_default;

typedef struct s_main_default
{
    std::string types;
    t_http_default http;
} t_main_default;

class Config
{
private:
    std::vector<Directive> _directives;
    Directive _parseDirective(const std::string &line);
    void _setBlock(std::ifstream &infile, std::vector<Directive> &directive, std::string pre_name);
    std::string trim(const std::string &str);
    std::map<std::string, std::string> _main;
    std::map<std::string, std::string> _http;
    std::map<std::string, std::string> _server;
    std::map<std::string, std::string> _location;
    void _setRelation();
    void _setIncludes();
    void _checkRealtion(std::vector<Directive> &directive);
    void _checkParent(std::string &parentName, std::string rightPre, std::string blockName) const;
    void _checkChildes(std::vector<Directive> &block, std::map<std::string, std::string> &blockFormat, std::string prarentBlockName);
    void _checkRepeatition(std::vector<Directive> &directives, std::string &parentName);
    void _checkValidValue(std::vector<Directive> &directives);
    void _checkEmpty(std::string &value, std::string directiveName, bool exist);
    bool _isFileExists(const std::vector<Directive> directives, const std::string &filePath, std::string directiveName, std::vector<Directive> &preDirective);
    bool _isDirectoryExists(const std::string &directoryPath, std::string directiveName);

public:
    Config();
    ~Config();
    void parsedConfig(int argc, char const **argv);
    void printDirectives(std::vector<Directive> directives, size_t tab);
    void getAllDirectives(std::vector<Directive> &newDirectives, const std::vector<Directive> &directives, const std::string dirName) const;
    const std::vector<Directive> &getDirectives() const;
    static std::vector<std::string> split(std::string input, char delimiter);
};

#endif
