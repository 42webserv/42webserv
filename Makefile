# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/14 09:52:20 by sunhwang          #+#    #+#              #
#    Updated: 2023/05/09 16:14:14 by chanwjeo         ###   ########.fr        #
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
DIR_SERVER	= server

SRC_DIRS	= $(sort $(addprefix $(DIR_SRC)/, $(DIR_CONFIG) $(DIR_ERROR) $(DIR_PARSE) $(DIR_PARSE)/HTTP $(DIR_PROCESS) $(DIR_SOCKET) $(DIR_UTILS) $(DIR_SERVER)))
SRC_INC_DIR	= $(addprefix -I, $(SRC_DIRS))
INC_DIR		= -I$(DIR_SRC) $(SRC_INC_DIR)

SRC_CONFIG	= Config CheckConfigValid
SRC_ERROR	= common_error
SRC_PARSE	= HTTPRequestParser HTTPResponse MimeTypesParser
SRC_PROCESS	= Master Worker
SRC_SOCKET	= Socket
SRC_UTILS	= Signal
SRC_SERVER	= Server

ifeq (,$(findstring bonus,$(MAKECMDGOALS)))
# SRC_WINDOW	+= draw_rt
else
# SRC_WINDOW	+= draw_rt_bonus
endif

SRCS_LIST = $(addprefix $(DIR_SRC)/,						\
			$(addprefix $(DIR_CONFIG)/,		$(SRC_CONFIG))	\
			$(addprefix $(DIR_ERROR)/,		$(SRC_ERROR))	\
			$(addprefix $(DIR_PARSE)/,		$(SRC_PARSE))	\
			$(addprefix $(DIR_PROCESS)/,	$(SRC_PROCESS))	\
			$(addprefix $(DIR_SOCKET)/,		$(SRC_SOCKET))	\
			$(addprefix $(DIR_SERVER)/,		$(SRC_SERVER))	\
			$(addprefix $(DIR_UTILS)/,		$(SRC_UTILS)))

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3
# CXXFLAGS	= -std=c++98 -g3


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

