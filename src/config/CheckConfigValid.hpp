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
public:
	CheckConfigValid() {}
	static bool Parse(const std::string &file_path)
	{
		std::ifstream file_stream(file_path);
		if (!file_stream.is_open())
		{
			std::cerr << "Failed to open file: " << file_path << std::endl;
			return false;
		}
		try
		{
			ParseStream(file_stream);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error parsing config file: " << e.what() << std::endl;
			return false;
		}
		return true;
	}

private:
	static void ParseStream(std::istream &input_stream)
	{
		std::string line;
		while (std::getline(input_stream, line))
		{
			line = Trim(line);
			if (line.empty())
			{
				continue;
			}
			if (line.front() == '#')
			{
				continue;
			}
			if (line.front() == '}')
			{
				if (_block_stack.empty())
				{
					throw std::runtime_error("Unexpected '}'");
				}
				_block_stack.pop();
				continue;
			}
			size_t found = line.find('{');
			if (found == std::string::npos)
			{
				if (_block_stack.empty())
				{
					throw std::runtime_error("Unexpected directive: " + line);
				}
			}
			else
			{
				// Block.
				if (found != line.size() - 1)
				{ //
					throw std::runtime_error("Unexpected character after '{'");
				}
				std::string block_name = line.substr(0, found);
				block_name = Trim(block_name);
				if (block_name.empty())
				{
					throw std::runtime_error("Empty block name");
				}
				_block_stack.push(block_name);
			}
		}
		if (!_block_stack.empty())
		{
			throw std::runtime_error("Missing '}'");
		}
	}

	static std::string Trim(const std::string &str)
	{ // 양쪽 끝의 공백 제거
		std::size_t first = str.find_first_not_of(' ');
		if (first == std::string::npos)
		{
			return "";
		}
		std::size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	static std::stack<std::string> _block_stack;
};

#endif // CheckConfigValid_HPP