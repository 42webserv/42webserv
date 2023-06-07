# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/14 09:52:20 by sunhwang          #+#    #+#              #
#    Updated: 2023/06/07 14:22:30 by chanwjeo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

OUT_DIR		= out/

DIR_SRC		= src
DIR_CONFIG	= config
DIR_ERROR	= error
DIR_PARSE	= parse
DIR_PROCESS	= process
DIR_SOCKET	= socket
DIR_UTILS	= utils
DIR_CGI		= cgi
DIR_SERVER	= server

SRC_DIRS	= $(sort $(addprefix $(DIR_SRC)/, $(DIR_CONFIG) $(DIR_ERROR) $(DIR_PARSE) $(DIR_PROCESS) $(DIR_SOCKET) $(DIR_UTILS) $(DIR_SERVER) $(DIR_CGI)))
SRC_INC_DIR	= $(addprefix -I, $(SRC_DIRS))
INC_DIR		= -I$(DIR_SRC) $(SRC_INC_DIR)

SRC_CONFIG	= CheckConfigValid Config DefaultConfig Directive
SRC_ERROR	= commonError
SRC_PARSE	= HTTPRequestParser MimeTypesParser
SRC_PROCESS	= commonProcess Master Worker Response
SRC_SOCKET	= Socket
SRC_UTILS	= Signal Utils
SRC_CGI		= CGI
SRC_SERVER	= Server ServerInfo

SRCS_LIST = $(addprefix $(DIR_SRC)/,						\
			$(addprefix $(DIR_CONFIG)/,		$(SRC_CONFIG))	\
			$(addprefix $(DIR_ERROR)/,		$(SRC_ERROR))	\
			$(addprefix $(DIR_PARSE)/,		$(SRC_PARSE))	\
			$(addprefix $(DIR_PROCESS)/,	$(SRC_PROCESS))	\
			$(addprefix $(DIR_SOCKET)/,		$(SRC_SOCKET))	\
			$(addprefix $(DIR_UTILS)/,		$(SRC_UTILS)) 	\
			$(addprefix $(DIR_CGI)/,		$(SRC_CGI)) 	\
			$(addprefix $(DIR_SERVER)/,		$(SRC_SERVER)))

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3
# CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

SRCS = $(addsuffix .cpp, $(DIR_SRC)/main $(SRCS_LIST))
OBJS = $(SRCS:%.cpp=$(OUT_DIR)%.o)

all: $(NAME)

bonus: $(NAME)

clean:
	$(RM) -r $(OUT_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJS): $(OUT_DIR)%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@
