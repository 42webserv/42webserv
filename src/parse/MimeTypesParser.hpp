/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypesParser.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 18:05:34 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/04 16:43:24 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MimeTypesParser_HPP
#define MimeTypesParser_HPP

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ParsedConfig.hpp"

/**
 * mime.types를 파싱해 각 확장자를 Content-Type에 맞게 반환해줌
 */
class MimeTypesParser
{
private:
    /* data */
    std::vector<Directive> includes;
    std::map<std::string, std::string> mimeMap;

    /*
     * A default constructor
     */
    MimeTypesParser();

    void parseMimeTypes(const std::string &filename);
    std::string getMimeTypesPath(std::vector<Directive> directive);

public:
    MimeTypesParser(ParsedConfig &config);

    /*
     * A copy constructor
     */
    MimeTypesParser(const MimeTypesParser &ref);

    /*
     * A assignment operator overload
     */
    MimeTypesParser &operator=(const MimeTypesParser &ref);

    /*
     * A destructor
     */
    ~MimeTypesParser();

    /*
     * Add it if you feel necessary additional member functions.
     */
    std::string getMimeType(const std::string &extension);
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // MimeTypesParser_HPP
