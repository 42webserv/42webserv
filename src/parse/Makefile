# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chanwjeo <chanwjeo@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/23 15:55:35 by chanwjeo          #+#    #+#              #
#    Updated: 2023/04/23 18:23:33 by chanwjeo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			= parse_test

CXXFLAGS			= -Wall -Wextra -Werror -std=c++98

# NOTE : Add Source directory here
# ------------------------------------------------------ #
# SRC_DIR			= src
HTTP_DIR			= HTTP

# NOTE : Add Source files here
# ------------------------------------------------------ #
# MAIN_SRC		= main

# NOTE : Add to SRC here
# ------------------------------------------------------ #
# SRC = $(addsuffix .c, $(addprefix $(MAIN_DIR), $(MAIN_SRC)))

ifeq (,$(findstring http,$(MAKECMDGOALS)))
SRC	= $(HTTP_DIR)/main $(HTTP_DIR)/HTTPRequestParser
else
SRC	= $(HTTP_DIR)/main $(HTTP_DIR)/HTTPRequestParser
endif


# ------------------------------------------------------ #

OBJ = $(SRC:cpp=o)

all: $(NAME)

# Colors
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
ORANGE = \033[38;5;208m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m
CUSTOM = \033[38;5;135m

#-----------------------------------------------------------------------
#    Mac compile option                                                |
#-----------------------------------------------------------------------
$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)╔══════════════════════════════════════════╗$(DEF_COLOR)"
	@echo "$(GREEN)║       parse_test compile finished.       ║$(DEF_COLOR)"
	@echo "$(GREEN)╚══════════════════════════════════════════╝$(DEF_COLOR)"

%.o: %.cpp
	@${CXX} ${CXXFLAGS} -c $< -o $@
	@echo "$(GREEN)Compiling... \t$< $(DEF_COLOR)"
# #-----------------------------------------------------------------------

clean:
	@rm -f $(OBJ)
	@echo "$(CUSTOM)ex02 obj files has been deleted.$(DEF_COLOR)"

fclean:
	@rm -f $(OBJ)
	@rm -f $(NAME)
	@echo "$(CUSTOM)ex02 archive files has been deleted.$(DEF_COLOR)"

re: fclean all
	@echo "$(CUSTOM)Cleaned and rebuilt ex02.$(DEF_COLOR)"

.PHONY: all clean fclean re
