#include "NginxConfig.hpp"

bool NginxConfig::Parse(const std::string &file_path)
{
	std::ifstream input_file(file_path);
	if (!input_file)
	{
		std::cerr << "Could not open config file " << file_path << std::endl;
		return false;
	}

	std::string line;
	std::string name, value;
	std::vector<Directive> *block_stack = &directives_;

	while (getline(input_file, line))
	{
		line = NginxConfig::Trim(line);
		if (line.empty())
		{
			continue;
		}

		if (line[0] == '#')
		{
			continue;
		}

		if (line[0] == '}')
		{
			block_stack = &(block_stack->back().block);
			continue;
		}

		std::istringstream line_stream(line);
		line_stream >> name;

		if (name.back() == '{')
		{
			name.pop_back();
			Directive d;
			d.name = name;
			d.value = "";
			d.block = std::vector<Directive>();
			;
			block_stack->push_back(d);
			block_stack = &(block_stack->back().block);
			continue;
		}

		line_stream >> value;
		name.pop_back(); // remove semicolon
		Directive d;
		d.name = name;
		d.value = "";
		d.block = std::vector<Directive>();
		;
		block_stack->push_back(d);
	}
	return true;
}

std::vector<Directive> &NginxConfig::GetDirectives()
{
	return directives_;
}

std::string NginxConfig::Trim(const std::string &str)
{
	const char *whitespace = " \t\n\r\f\v";
	size_t start = str.find_first_not_of(whitespace);
	size_t end = str.find_last_not_of(whitespace);
	if (start == std::string::npos || end == std::string::npos)
	{
		return "";
	}
	return str.substr(start, end - start + 1);
}

void NginxConfig::printConfig(std::vector<Directive> directive)
{
	for (size_t i = 0; i < directive.size(); i++)
	{
		std::cout << "  " << directive[i].name << ": " << directive[i].value << std::endl;
		if (directive[i].block.empty())
			continue;
		std::cout << "  block: " << std::endl;
		NginxConfig::printConfig(directive[i].block);
	}
}