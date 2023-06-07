/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commonConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:12:13 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/02 19:53:12 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_CONFIG_HPP
#define COMMON_CONFIG_HPP

// Configurations
#define DEFAULT_PORT 80
#define DEFAULT_SERVER_NAME "localhost"
#define DEFAULT_SERVER_ROOT "./"
#define DEFAULT_SERVER_INDEX "index.html"
#define DEFAULT_SERVER_AUTOINDEX false
#define DEFAULT_SERVER_ERROR_PAGE "error.html"

// HTTP
// #define HTTP_VERSION "HTTP/1.1"
#define CRLF "\r\n"
#define CRLF2 "\r\n\r\n"
#define SP " "
#define COLON ":"
#define SEMICOLON ";"
#define QUESTION "?"
#define SLASH "/"
#define DOT "."
#define COMMA ","
#define DASH "-"
#define UNDERSCORE "_"
#define ASTERISK "*"
#define EQUAL "="
#define AMPERSAND "&"
#define HASH "#"
#define TILDE "~"
#define VERTICAL_BAR "|"
#define EXCLAMATION_MARK "!"
#define PLUS "+"
#define PERCENT "%"
#define DOLLAR "$"
#define AT "@"
#define CARET "^"
#define BACKSLASH "\\"
#define QUOTATION "\""
#define SINGLE_QUOTATION "'"
#define LEFT_PARENTHESIS "("
#define RIGHT_PARENTHESIS ")"
#define LEFT_BRACKET "["
#define RIGHT_BRACKET "]"
#define LEFT_BRACE "{"
#define RIGHT_BRACE "}"
#define LESS_THAN "<"
#define GREATER_THAN ">"
#define NEWLINE "\n"
#define TAB "\t"

// Extensions
#define HTML_EXTENSION ".html"
#define CSS_EXTENSION ".css"
#define JS_EXTENSION ".js"
#define ICO_EXTENSION ".ico"

// directives

#define MAIN_DIRECTIVE "main"
#define HTTP_DIRECTIVE "http"
#define SERVER_DIRECTIVE "server"
#define LOCATION_DIRECTIVE "location"
#define INCLUDE_DIRECTIVE "include"
#define INDEX_DIRECTIVE "index"
#define AUTOINDEX_DIRECTIVE "autoindex"
#define LIMIT_EXCEPT_DIRECTIVE "limit_except"
#define RETURN_DIRECTIVE "return"
#define LISTEN_DIRECTIVE "listen"
#define SERVER_NAME_DIRECTIVE "server_name"
#define ERROR_PAGE_DIRECTIVE "error_page"
#define CLIENT_MAX_BODY_SIZE_DIRECTIVE "client_max_body_size"
#define ROOT_DIRECTIVE "root"
#define TYPES_DIRECTIVE "types"
#define CGI_EXTENSION_DIRECTIVE "cgi_extension"
#define CGI_PATH_DIRECTIVE "cgi_path"
#define DEFAULT_TYPE_DIRECTIVE "default_type"
#define SEND_FILE_DIRECTIVE "send_file"
#define TCP_NOPUSH_DIRECTIVE "tcp_nopush"
#define ACCESS_LOG_DIRECTIVE "access_log"
#define ERROR_LOG_DIRECTIVE "error_log"
#define WORKER_PROCESS_DIRECTIVE "worker_process"
#define USER_DIRECTIVE "user"
#define PID_DIRECTIVE "pid"

// directive values
#define DEFALUT_DIRECTIVE_VALUE_FAIL "fail"

// Default values
#define DEFAULT_CLIENT_MAX_BODY_SIZE 1000000

#endif
