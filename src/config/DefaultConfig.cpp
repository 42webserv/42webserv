/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:59:10 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/21 19:48:25 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// http://nginx.org/en/docs/ngx_core_module.html

/**
 * Full Example Configuration에 나온 지시자들
 * 모두 main 아래 소속되어 있다. 바로 위 부모를 해당 지시자의 Context로 이해한다.
 *
 * 블록 지시자들
 * events
 * http -> server -> location
 * http -> upstream
 */

#include <utility>
#include "DefaultConfig.hpp"
#include "Directive.hpp"
#include "common_error.hpp"

#define ERROR_DIRECTIVE_NAME "Error: Invalid name to "
#define ERROR_DIRECTIVE_SIZE "Error: Invalid size to "

DefaultConfig::DefaultConfig(Config &config) : config(config) {}

void DefaultConfig::addAndCheckChildDirectives(Directive &dir, std::vector<Directive> &dirs, const std::string &name, void (DefaultConfig::*fn_addDirs)(Directive &dir, const std::string name), void (DefaultConfig::*fn_checkDirs)(std::vector<Directive> &dirs, const std::string pre_name))
{
	if (dir.name == name)
	{
		if (fn_addDirs)
			(this->*fn_addDirs)(dir, name);
		if (fn_checkDirs)
			(this->*fn_checkDirs)(dirs, name);
	}
	else
		stderr_exit(ERROR_DIRECTIVE_NAME + name);
}

void DefaultConfig::checkDirectives()
{
	std::vector<Directive> dirs = config.getDirectives();

	checkMainDirectives(dirs);
}

void DefaultConfig::addDirectives(std::map<std::string, std::string> &dirs, Directive &dir, const std::string name)
{
	std::pair<std::string, std::string> p;
	for (std::map<std::string, std::string>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		p = *it;
		if (!hasDirInBlock(dir, p.first))
			dir.block.push_back(newDir(p.first, p.second, name));
	}
}

void DefaultConfig::addMainDirectives(Directive &main, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair("user", "nobody nobody"),
		std::make_pair("worker_processes", "1"),
		std::make_pair("error_log", "logs/error.log error"),
		std::make_pair("pid", "logs/nginx.pid"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, main, name);
}

void DefaultConfig::checkMainDirectives(std::vector<Directive> &dirs)
{
	const std::string name = "main";
	std::vector<Directive> tmp;
	Directive dir;

	config.getAllDirectives(tmp, dirs, name);
	if (tmp.size() == 0)
	{
		dir = newDir(name, "", name); // TODO Config.cpp에도 동일하게 하고 있음.
		tmp.insert(tmp.begin(), dir);
	}
	if (tmp.size() == 1)
	{
		dir = tmp.front();
		addAndCheckChildDirectives(dir, dirs, name, &DefaultConfig::addMainDirectives, &DefaultConfig::checkHttpDirectives);
	}
	else
		stderr_exit(ERROR_DIRECTIVE_SIZE + name);
}

void DefaultConfig::addHttpDirectives(Directive &http, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair("include", "./assets/conf/mime.types"),
		// std::make_pair("include", "/etc/nginx/proxy.conf"),
		// std::make_pair("include", "/etc/nginx/fastcgi.conf"),
		std::make_pair("index", "index.html index.htm index.php"),
		std::make_pair("default_type", "application/octet-stream"),
		std::make_pair("sendfile", "on"),
		std::make_pair("tcp_nopush", "on"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, http, name);
}

void DefaultConfig::checkHttpDirectives(std::vector<Directive> &dirs, const std::string pre_name)
{
	const std::string name = "http";
	std::vector<Directive> tmp;
	Directive dir;

	config.getAllDirectives(tmp, dirs, name);
	(void)pre_name;
	// if (tmp.size() == 0)
	// {
	// 	dir = newDir(name, "", pre_name);
	// 	tmp.insert(tmp.begin(), dir);
	// }
	if (tmp.size() == 1)
	{
		dir = tmp.front();
		addAndCheckChildDirectives(dir, dirs, name, &DefaultConfig::addHttpDirectives, &DefaultConfig::checkServerDirectives);
	}
	else
		stderr_exit(ERROR_DIRECTIVE_SIZE + name);
}

void DefaultConfig::addServerDirectives(Directive &server, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair("listen", "80"),
		std::make_pair("server_name", "domain1.com www.domain1.com"),
		std::make_pair("access_log", "logs/domain1.access.log  main"),
		std::make_pair("root", "html"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, server, name);
}

void DefaultConfig::checkServerDirectives(std::vector<Directive> &dirs, const std::string pre_name)
{
	const std::string name = "server";
	std::vector<Directive> tmp;
	Directive dir;

	config.getAllDirectives(tmp, dirs, name);
	(void)pre_name;
	// if (0 == tmp.size())
	// {
	// 	dir = newDir(name, "", pre_name);
	// 	tmp.insert(tmp.begin(), dir);
	// }
	if (0 < tmp.size())
	{
		for (std::vector<Directive>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			dir = *it;
			addAndCheckChildDirectives(dir, dirs, name, &DefaultConfig::addServerDirectives, &DefaultConfig::checkLocationDirectives);
		}
	}
}

void DefaultConfig::addLocationDirectives(Directive &location, const std::string name)
{
	std::map<std::string, std::string> dirs;
	std::pair<std::string, std::string> ps[] = {
		std::make_pair("", ""),
	};

	(void)location;
	(void)name;
	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	// addDirectives(dirs, location, name);
}

void DefaultConfig::checkLocationDirectives(std::vector<Directive> &dirs, const std::string pre_name)
{
	const std::string name = "location";
	std::vector<Directive> tmp;
	Directive dir;

	config.getAllDirectives(tmp, dirs, name);
	(void)pre_name;
	// if (0 == tmp.size())
	// {
	// 	dir = newDir(name, "", pre_name);
	// 	tmp.insert(tmp.begin(), dir);
	// }
	if (0 < tmp.size())
	{
		for (std::vector<Directive>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			dir = *it;
			addAndCheckChildDirectives(dir, dirs, name, &DefaultConfig::addLocationDirectives, NULL);
		}
	}
}

DefaultConfig::~DefaultConfig() {}
