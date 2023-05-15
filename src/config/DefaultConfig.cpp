/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 14:59:10 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/15 18:28:24 by chanwjeo         ###   ########.fr       */
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

Directive newDir(const std::string name, const std::string value)
{
	Directive dir = Directive();

	dir.name = name;
	dir.value = value;

	return dir;
}

void addServerDirective(Directive &http, std::string type)
{
	Directive server = newDir("server", "");
	Directive location = newDir("location", "");
	if (type == "php")
	{
		std::pair<std::string, std::string> p[] = {
			std::make_pair("listen", "80"),
			std::make_pair("server_name", "domain1.com www.domain1.com"),
			std::make_pair("access_log", "logs/domain1.access.log  main"),
			std::make_pair("root", "html"),
		};
		for (size_t i = 0; i < 4; i++)
			server.block.push_back(newDir(p[i].first, p[i].second));

		location.value = "~ \\.php$";
		location.block.push_back(newDir("fastcgi_pass", "127.0.0.1:1025"));
	}
	else if (type == "reverse-proxy")
	{
		std::pair<std::string, std::string> p[] = {
			std::make_pair("listen", "80"),
			std::make_pair("server_name", "domain2.com www.domain2.com"),
			std::make_pair("access_log", "logs/domain2.access.log  main"),
		};
		for (size_t i = 0; i < 4; i++)
			server.block.push_back(newDir(p[i].first, p[i].second));

		location.value = "~ ^/(images|javascript|js|css|flash|media|static)/";
		location.block.push_back(newDir("root", "/var/www/virtual/big.server.com/htdocs"));
		location.block.push_back(newDir("expires", "30d"));
		server.block.push_back(location);

		location.value = "/";
		location.block.push_back(newDir("proxy_pass", "http://127.0.0.1:8080"));
	}
	else if (type == "load-balancing")
	{
		std::pair<std::string, std::string> p[] = {
			std::make_pair("listen", "80"),
			std::make_pair("server_name", "big.server.com"),
			std::make_pair("access_log", "logs/big.server.access.log main"),
		};
		for (size_t i = 0; i < 4; i++)
			server.block.push_back(newDir(p[i].first, p[i].second));

		location.value = "/";
		location.block.push_back(newDir("proxy_pass", "http://big_server_com"));
	}
	server.block.push_back(location);
	http.block.push_back(server);
}

void addHttpDirectives(Directive &main)
{
	Directive http = newDir("http", "");
	std::pair<std::string, std::string> p[] = {
		std::make_pair("include", "conf/mime.types"),
		std::make_pair("include", "/etc/nginx/proxy.conf"),
		std::make_pair("include", "/etc/nginx/fastcgi.conf"),
		std::make_pair("index", "index.html index.htm index.php"),
		std::make_pair("default_type", "application/octet-stream"),
		// TODO log_format value 처리가 까다로움 실제 예시
		// main '$remote_addr - $remote_user [$time_local]  $status '
		// '"$request" $body_bytes_sent "$http_referer" '
		// '"$http_user_agent" "$http_x_forwarded_for"';
		std::make_pair("log_format", "main '$remote_addr - $remote_user [$time_local]  $status '"),
		std::make_pair("access_log", "logs/access.log  main"),
		std::make_pair("sendfile", "on"),
		std::make_pair("tcp_nopush", "on"),
		std::make_pair("server_names_hash_bucket_size", "128"),
	};
	for (size_t i = 0; i < 7; i++)
		http.block.push_back(newDir(p[i].first, p[i].second));

	addServerDirective(http, "php");
	addServerDirective(http, "reverse-proxy");
	addServerDirective(http, "load-balancing");
	main.block.push_back(http);
}

void addMainDirectives(Directive &main)
{
	std::pair<std::string, std::string> p[] = {
		std::make_pair("user", "www www"),
		std::make_pair("worker_processes", "5"),
		std::make_pair("error_log", "logs/error.log"),
		std::make_pair("pid", "logs/nginx.pid"),
		std::make_pair("worker_rlimit_nofile", "8192"),
	};

	for (size_t i = 0; i < 5; i++)
		main.block.push_back(newDir(p[i].first, p[i].second));

	Directive events = newDir("events", "");
	events.block.push_back(newDir("worker_connections", "4096"));
	main.block.push_back(events);

	addHttpDirectives(main);
}

DefaultConfig::DefaultConfig()
{
	this->_directives.clear();
	Directive main = newDir("main", "");

	addMainDirectives(main);
	this->_directives.push_back(main);
}

DefaultConfig::~DefaultConfig()
{
	this->_directives.clear();
}

void DefaultConfig::setDefaults()
{
}
