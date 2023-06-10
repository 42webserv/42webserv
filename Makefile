# # **************************************************************************** #
# #                                                                              #
# #                                                         :::      ::::::::    #
# #    Makefile                                           :+:      :+:    :+:    #
# #                                                     +:+ +:+         +:+      #
# #    By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+         #
# #                                                 +#+#+#+#+#+   +#+            #
# #    Created: 2023/04/14 09:52:20 by sunhwang          #+#    #+#              #
# #    Updated: 2023/06/10 00:36:56 by chanwjeo         ###   ########.fr        #
# #                                                                              #
# # **************************************************************************** #

SHELL = bash

NAME = webserv

#color
BOLD		= \033[1m
RED			= \033[91m
GREEN 		= \033[92m
DBLUE 		= \033[94m
CYAN		= \033[96m

OUT_DIR			= out/

DIR_SRC			= src
DIR_CONFIG		= config
DIR_ERROR		= error
DIR_PARSE		= parse
DIR_PROCESS		= process
DIR_SOCKET		= socket
DIR_UTILS		= utils
DIR_CGI			= cgi
DIR_SERVER		= server
DIR_COLOR		= color

SRC_DIRS		= $(sort $(addprefix $(DIR_SRC)/, $(DIR_CONFIG) $(DIR_ERROR) $(DIR_PARSE) $(DIR_PROCESS) $(DIR_SOCKET) $(DIR_UTILS) $(DIR_SERVER) $(DIR_CGI) $(DIR_COLOR)))

SRC_INC_DIR		= $(addprefix -I, $(SRC_DIRS))
INC_DIR			= -I$(DIR_SRC) $(SRC_INC_DIR)

SRC_CONFIG		= CheckConfigValid Config DefaultConfig Directive
SRC_ERROR		= commonError
SRC_PARSE		= HTTPRequestParser MimeTypesParser
SRC_PROCESS		= Master Worker Response
SRC_SOCKET		= Socket
SRC_UTILS		= Signal Utils
SRC_CGI			= CGI
SRC_SERVER		= Server ServerInfo


SRCS_LIST		= $(addprefix $(DIR_SRC)/,						\
					$(addprefix $(DIR_CONFIG)/,		$(SRC_CONFIG))	\
					$(addprefix $(DIR_ERROR)/,		$(SRC_ERROR))	\
					$(addprefix $(DIR_PARSE)/,		$(SRC_PARSE))	\
					$(addprefix $(DIR_PROCESS)/,	$(SRC_PROCESS))	\
					$(addprefix $(DIR_SOCKET)/,		$(SRC_SOCKET))	\
					$(addprefix $(DIR_UTILS)/,		$(SRC_UTILS)) 	\
					$(addprefix $(DIR_CGI)/,		$(SRC_CGI)) 	\
					$(addprefix $(DIR_SERVER)/,		$(SRC_SERVER))	\
					$(addprefix $(DIR_COLOR)/,		$(SRC_COLOR)))	\


CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3
# CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

SRCS = $(addsuffix .cpp, $(DIR_SRC)/main $(SRCS_LIST))
OBJS = $(SRCS:%.cpp=$(OUT_DIR)%.o)

INDEX = 0
TOTAL = $(words $(SRCS))
PROGRESS_BAR_WIDTH = 40

define print_progress
	@printf "$(GREEN)\rCreating $(DBLUE)$(NAME) $(CYAN)[%d/%d]" $(INDEX) $(TOTAL); \
	printf "["; \
	for ((i=0; i<$(PROGRESS_BAR_WIDTH); i++)); do \
		if [[ $$((i * $(TOTAL) / $(PROGRESS_BAR_WIDTH))) -lt $(INDEX) ]]; then \
			printf "$(CYAN)■$(RESET)"; \
		else \
			printf " "; \
		fi; \
	done; \
	printf "$(CYAN)] %.1f%%" $$(expr \( $(INDEX) \) \* 100 / $(TOTAL))
endef

all: $(NAME)


bonus: $(NAME)


clean:
	@$(RM) -r $(OUT_DIR)
	@echo -e "$(RED)🗑️ $(BOLD)$(NAME)$(BOLD) objects removed successfully $(GREEN)🗑️"

fclean:
	@$(RM) -r $(OUT_DIR)
	@$(RM) $(NAME)
		@echo -e "$(RED)🗑️ $(BOLD)$(NAME)$(BOLD) removed successfully $(GREEN)🗑️"


re: fclean
	@$(MAKE) all



INDEX = 0
TOTAL = $(words $(SRCS))

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo -e ""
	@echo -e "$(GREEN)🚀 $(BOLD)$(NAME)$(GREEN) compiled successfully!🚀"


$(OBJS): $(OUT_DIR)%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@
	$(eval INDEX = $(shell expr $(INDEX) + 1))
	$(call print_progress)

.PHONY: all clean fclean re
