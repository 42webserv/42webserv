/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypesParser.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 18:05:34 by chanwjeo          #+#    #+#             */
/*   Updated: 2023/05/03 18:08:06 by chanwjeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MimeTypesParser_HPP
#define MimeTypesParser_HPP

#include <iostream>
#include <string>

class MimeTypesParser
{
private:
    /* data */

public:
    /*
     * A default constructor
     */
    MimeTypesParser();

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
};

/*
 * Add it if you feel necessary additional functions.
 */

#endif // MimeTypesParser_HPP
