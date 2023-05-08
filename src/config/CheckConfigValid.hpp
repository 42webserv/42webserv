/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckConfigValid.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 14:46:04 by seokchoi          #+#    #+#             */
/*   Updated: 2023/04/28 21:21:43 by seokchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CheckConfigValid_HPP
#define CheckConfigValid_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <stack>

class CheckConfigValid
{
private:
	static std::stack<std::string> _block_stack;
	static void ParseStream(std::istream &input_stream);
	static std::string Trim(const std::string &str);

public:
	CheckConfigValid();
	~CheckConfigValid();
	static bool Parse(const std::string &file_path);
};

#endif // CheckConfigValid_HPP