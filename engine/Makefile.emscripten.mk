NAME = figment

SRCDIR = src
IMGUIDIR = lib/imgui

WEB_DIR = ../app/src/renderer
WEB_EXE = $(WEB_DIR)/index.html
ASSET_DIR = res

SRC_DIRS := $(shell find src lib/imgui -type d | grep -v -E "src/platform/macos|src/scripting")

OBJ_DIR := build/objs

SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

INCL = -I src -I lib/imgui/include -I lib/glm -I lib -I lib/entt -I src/scene -I src/physics -I src/platform/opengl -I /Users/willehard/dev/emsdk/upstream/emscripten

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

EMS_OPTIONS = -s USE_GLFW=3
EMS_OPTIONS += -s DISABLE_EXCEPTION_CATCHING=0 -sEXPORTED_FUNCTIONS=_onMouseMove,_insertObject,_onCanvasResize,_main -s EXPORTED_RUNTIME_METHODS='["cwrap", "allocate", "intArrayFromString", "ALLOC_NORMAL", "UTF8ToString"]'

CFLAGS = -std=c++17 #-Wall -Wextra -Werror
EMS_CFLAGS = ${CFLAGS} -DIMGUI_DISABLE_FILE_FUNCTIONS -sMODULARIZE=1 -sEXPORT_ES6=1 ${EMS_OPTIONS}
LDFLAGS = ${EMS_OPTIONS} -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 #-lGLEW -lglfw

CC = em++

all: $(NAME)

$(NAME): ${OBJS} ${WEB_DIR}
	@printf "Building WASM %s\n" "$(NAME)"
	$(CC) $(EMS_CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(WEB_EXE) -O3 --shell-file index.html --preload-file ${ASSET_DIR}@/res
	@mv ${WEB_DIR}/index.data ../app/public/index.data
	@printf "done.\n"

$(OBJ_DIR)/%.o: %.cpp
	@printf "Compiling %s -> %s\n" "$<" "$@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@
	
$(WEB_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -Rf $(WEB_DIR)
	@rm -f $(NAME)
	@rm -rf build

re: fclean all

.PHONY: all clean fclean re
