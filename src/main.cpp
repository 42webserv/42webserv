/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/03 21:39:38 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DefaultConfig.hpp"
#include "Master.hpp"
#include "Worker.hpp"
#include "Config.hpp"
#include "CheckConfigValid.hpp"
#include "ParsedConfig.hpp"

std::map<std::string, std::string> parseMimeTypes(const std::string &filename)
{
    std::ifstream file(filename);
    std::map<std::string, std::string> mimeMap;

    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            line = line.substr(0, line.length() - 1); // mime.types 각 줄 끝의 ; 제거
            if (line[0] != '#' && !line.empty())
            {
                std::vector<std::string> tokens;
                std::istringstream iss(line);
                std::string token;

                std::cout << line << std::endl;

                while (iss >> token)
                    tokens.push_back(token);

                if (tokens.size() > 1)
                {
                    std::string mime = tokens[0];
                    for (size_t i = 1; i < tokens.size(); i++)
                        mimeMap[tokens[i]] = mime;
                }
            }
        }
        file.close();
    }

    return mimeMap;
}

std::string getMimeType(const std::string &filename, std::map<std::string, std::string> &mimeMap)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    std::cout << extension << std::endl;
    for (std::map<std::string, std::string>::iterator iter = mimeMap.begin(); iter != mimeMap.end(); iter++)
        if (iter->first == extension)
            return iter->second;
    return "application/octet-stream";
}

std::string getMimeTypes(std::vector<Directive> directive)
{
    for (size_t i = 0; i < directive.size(); i++)
        if (directive[i].name == "include")
            if (directive[i].value.substr(directive[i].value.length() - 11) == "mime.types;")
                return directive[i].value;
    return "";
}

int main(int argc, char const *argv[])
{
    // Nginx Config file parsing
    ParsedConfig config;
    config.parsedConfig(argc, argv);
    // config.printDirectives(config.getDirectives(), 0); // Directive 출력해보기
    std::vector<Directive> includes;
    config.getAllDirectives(includes, config.getDirectives(), "include");
    // config.printDirectives(servers, 0);
    std::string mimeTypes = "./assets/" + getMimeTypes(includes).substr(0, getMimeTypes(includes).length() - 1);
    std::cout << mimeTypes << std::endl;
    std::map<std::string, std::string> mimeMap = parseMimeTypes(mimeTypes);
    std::string extension = "jar";
    std::string mimeType = getMimeType(extension, mimeMap);
    std::cout << mimeType << std::endl;

    // Master master;
    // Worker worker(master);
    // worker.run();
    return 0;
}
