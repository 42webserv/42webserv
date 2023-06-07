/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seokchoi <seokchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:59:10 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/30 16:34:48 by seokchoi         ###   ########.fr       */
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
#include "commonError.hpp"
#include "DefaultConfig.hpp"
#include "Directive.hpp"
#include "commonConfig.hpp"

#define ERROR_DIRECTIVE_NAME "Error: Invalid name to "
#define ERROR_DIRECTIVE_SIZE "Error: Invalid size to "

DefaultConfig::DefaultConfig(std::vector<Directive> &dirs) : _dirs(dirs) {}

void DefaultConfig::checkDirectives()
{
	checkMainDirectives(_dirs);
}

void DefaultConfig::checkDirectives(std::vector<Directive> &dirs, const std::string name, void (DefaultConfig::*fn_addDirs)(Directive &dir, const std::string name), void (DefaultConfig::*fn_checkDirs)(std::vector<Directive> &dirs))
{
	for (std::vector<Directive>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		Directive &dir = *it;
		if (dir.name == name)
		{
			if (fn_addDirs)
				(this->*fn_addDirs)(dir, name);
			if (fn_checkDirs)
				(this->*fn_checkDirs)(dirs);
		}
	}
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

void DefaultConfig::addDirectives(std::vector<Directive> &dirs, Directive &dir, const std::string name)
{
	Directive d;
	for (std::vector<Directive>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		d = *it;
		if (d.pre_name != name)
			d.pre_name = name;
		if (!hasDirInBlock(dir, d.name))
			dir.block.push_back(d);
	}
}

void DefaultConfig::addMainDirectives(Directive &main, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair(USER_DIRECTIVE, "nobody"),
		std::make_pair(WORKER_PROCESS_DIRECTIVE, "1"),
		std::make_pair(ERROR_LOG_DIRECTIVE, "logs/error.log error"),
		std::make_pair(PID_DIRECTIVE, "logs/nginx.pid"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, main, name);
}

void DefaultConfig::checkMainDirectives(std::vector<Directive> &dirs)
{
	const std::string name = MAIN_DIRECTIVE;

	if (dirs.size() == 0)
		dirs.insert(dirs.begin(), newDir(name, "", name));
	else if (dirs.size() == 1)
	{
		Directive &dir = dirs.front();
		if (dir.name != name)
		{
			Directive tmp = newDir(name, "", name);
			addDirectives(dirs, tmp, name);
			dirs.erase(dirs.begin(), dirs.end());
			dirs.insert(dirs.begin(), tmp);
		}
	}
	else
		stderrExit(ERROR_DIRECTIVE_SIZE + name);

	Directive &dir = dirs.front();
	addMainDirectives(dir, name);
	checkHttpDirectives(dir.block);
}

void DefaultConfig::addHttpDirectives(Directive &http, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair(INCLUDE_DIRECTIVE, "./assets/conf/mime.types"),
		// std::make_pair(INCLUDE_DIRECTIVE, "/etc/nginx/proxy.conf"),
		// std::make_pair(INCLUDE_DIRECTIVE, "/etc/nginx/fastcgi.conf"),
		std::make_pair(INDEX_DIRECTIVE, "index.html index.htm index.php"),
		std::make_pair(DEFAULT_TYPE_DIRECTIVE, "application/octet-stream"),
		std::make_pair(SEND_FILE_DIRECTIVE, "on"),
		std::make_pair(TCP_NOPUSH_DIRECTIVE, "on"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, http, name);
}

void DefaultConfig::checkHttpDirectives(std::vector<Directive> &dirs)
{
	const std::string name = HTTP_DIRECTIVE;

	for (std::vector<Directive>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		Directive &dir = *it;
		if (dir.name == name)
			checkDirectives(dir.block, name, &DefaultConfig::addHttpDirectives, &DefaultConfig::checkServerDirectives);
	}
}

void DefaultConfig::addServerDirectives(Directive &server, const std::string name)
{
	std::map<std::string, std::string> dirs;
	const std::pair<std::string, std::string> ps[] = {
		std::make_pair(LISTEN_DIRECTIVE, "80"),
		std::make_pair(SERVER_NAME_DIRECTIVE, "domain1.com www.domain1.com"),
		std::make_pair(ACCESS_LOG_DIRECTIVE, "logs/domain1.access.log  main"),
		std::make_pair(ROOT_DIRECTIVE, "html"),
	};

	setDirectivesToMap(dirs, ps, sizeof(ps) / sizeof(ps[0]));
	addDirectives(dirs, server, name);
}

void DefaultConfig::checkServerDirectives(std::vector<Directive> &dirs)
{
	const std::string name = SERVER_DIRECTIVE;

	for (std::vector<Directive>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		Directive &dir = *it;
		if (dir.name == name)
			checkDirectives(dir.block, name, &DefaultConfig::addServerDirectives, &DefaultConfig::checkLocationDirectives);
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

void DefaultConfig::checkLocationDirectives(std::vector<Directive> &dirs)
{
	const std::string name = LOCATION_DIRECTIVE;

	for (std::vector<Directive>::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		Directive &dir = *it;
		if (dir.name == name)
			checkDirectives(dir.block, name, &DefaultConfig::addLocationDirectives, NULL);
	}
}

DefaultConfig::~DefaultConfig() {}
