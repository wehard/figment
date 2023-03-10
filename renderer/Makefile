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

WEB_DIR = ../app/src/renderer
WEB_EXE = $(WEB_DIR)/index.html

ASSET_DIR = res

# Define the directories where source files are located
SRC_DIRS := $(shell find src lib/imgui -type d)

# Define the directory where object files will be placed
OBJ_DIR := build/objs

# Define the directory where the final executable will be placed
BIN_DIR := build/bin

# Get a list of all the source files in the source directories
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Generate a list of object files to build
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Define the name of the final executable
BIN := $(BIN_DIR)/myprogram

CFLAGS = -std=c++17 #-Wall -Wextra -Werror

INCL = -I include -I lib/imgui/include -I lib/glm -I lib -I lib/entt -I src/scene -I src/physics

LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo 
LDFLAGS += -lm

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
EMS = -s USE_GLFW=3
EMS += -s DISABLE_EXCEPTION_CATCHING=0 -sEXPORTED_FUNCTIONS=_onMouseMove,_insertObject,_onCanvasResize,_main -s EXPORTED_RUNTIME_METHODS='["cwrap", "allocate", "intArrayFromString", "ALLOC_NORMAL", "UTF8ToString"]'

EMS_CFLAGS = ${CFLAGS} -DIMGUI_DISABLE_FILE_FUNCTIONS -sMODULARIZE=1 -sEXPORT_ES6=1 ${EMS}
EMS_INCL = ${INCL} 
EMS_LDFLAGS = -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 ${EMS}

CC = em++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "compiling \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME) -O3

ems: ${OBJS} ${WEB_DIR}
	@$(CC) $(EMS_CFLAGS) $(EMS_INCL) $(OBJS) $(EMS_LDFLAGS) -o $(WEB_EXE) -O3 --shell-file index.html --preload-file ${ASSET_DIR}@
	@mv ${WEB_DIR}/index.data ../app/public/index.data

$(OBJ_DIR)/%.o: %.cpp
	@printf "Compiling %s -> %s\n" "$<" "$@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@ -O3

$(WEB_DIR):
	@mkdir -p $@

debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -Rf $(WEB_DIR)
	@rm -f $(NAME)
	@rm -rf build

re: fclean all

.PHONY: all clean fclean re debug
