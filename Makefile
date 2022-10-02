# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wkorande <willehard@gmail.com>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/06/01 15:48:04 by rjaakonm          #+#    #+#              #
#    Updated: 2022/06/20 17:18:34 by wkorande         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = figment

SRCDIR = src
IMGUIDIR = lib/imgui

SRC = Application.cpp\
	Camera.cpp\
	GLContext.cpp\
	GLObject.cpp\
	GLRenderer.cpp\
	GUIContext.cpp\
	Shader.cpp\
	main.cpp

IMGGUI_SRC = imgui.cpp\
	imgui_demo.cpp\
	imgui_draw.cpp\
	imgui_impl_sdl.cpp\
	imgui_impl_opengl3.cpp\
	imgui_tables.cpp\
	imgui_widgets.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRC)) $(addprefix $(IMGUIDIR)/, $(IMGGUI_SRC))
OBJS = $(SRCS:.cpp=.o)




CFLAGS = -std=c++17 #-Wall -Wextra -Werror

INCL = -I include -I lib/imgui/include -I lib/glm -I lib -I /opt/homebrew/include `sdl2-config --cflags`
INCL += -I /usr/local/include -I /opt/local/include

LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
LDFLAGS += -lm

CC = clang++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "compiling \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME) -O3

%.o: %.cpp
	@mkdir -p out
	@printf "compiling %s -> %s\n" "$<" "$@"
	@$(CC) $(CFLAGS) $(INCL) -c $< -o  $@ -O3

debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	@rm -rf $(OBJS)
	@rm -Rf $(OUTDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
