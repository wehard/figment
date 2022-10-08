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

WEB_DIR = web
WEB_EXE = $(WEB_DIR)/index.html

SRC = Camera.cpp\
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

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
EMS = -s USE_SDL=2
EMS += -s DISABLE_EXCEPTION_CATCHING=1 -sEXPORTED_FUNCTIONS=_onMouseMove,_main

EMS_CFLAGS = ${CFLAGS} -DIMGUI_DISABLE_FILE_FUNCTIONS ${EMS}
EMS_INCL = ${INCL} 
EMS_LDFLAGS = -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s NO_FILESYSTEM=1 ${EMS}

CC = em++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "compiling \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME) -O3

ems: ${OBJS} ${WEB_DIR}
	@$(CC) $(EMS_CFLAGS) $(EMS_INCL) $(OBJS) $(EMS_LDFLAGS) -o $(WEB_EXE) -O3 --shell-file index.html

%.o: %.cpp
	@printf "compiling %s -> %s\n" "$<" "$@"
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@ -O3

$(WEB_DIR):
	@mkdir -p $@

debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	@rm -rf $(OBJS)
	@rm -Rf $(WEB_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
