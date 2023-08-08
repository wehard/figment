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

CFLAGS = -g -std=c++17 #-Wall -Wextra -Werror

INCL = -I src -I lib/imgui/include -I lib/glm -I lib -I lib/entt -I src/scene -I src/physics -I src/platform/opengl -I /opt/homebrew/include
INCL += -I /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native
INCL += -I src/scripting

INCL += -I lib/mono/include

LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -L /opt/homebrew/lib -lglfw -L /opt/homebrew/lib -lGLEW
LDFLAGS += -lm
LDFLAGS += -L /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native -lnethost
LDFLAGS += -rpath /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native
LDFLAGS += -L/opt/homebrew/opt/zlib/lib -lz -liconv
LDFLAGS += -L lib/mono/lib -lmonosgen-2.0

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
EMS = -s USE_GLFW=3
EMS += -s DISABLE_EXCEPTION_CATCHING=0 -sEXPORTED_FUNCTIONS=_onMouseMove,_insertObject,_onCanvasResize,_main -s EXPORTED_RUNTIME_METHODS='["cwrap", "allocate", "intArrayFromString", "ALLOC_NORMAL", "UTF8ToString"]'

EMS_CFLAGS = ${CFLAGS} -DIMGUI_DISABLE_FILE_FUNCTIONS -sMODULARIZE=1 -sEXPORT_ES6=1 ${EMS}
EMS_INCL = ${INCL} 
EMS_LDFLAGS = -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 ${EMS}

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)

#CC = em++
CC = clang++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "Building \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME)

ems: ${OBJS} ${WEB_DIR}
	@$(CC) $(EMS_CFLAGS) $(EMS_INCL) $(OBJS) $(EMS_LDFLAGS) -o $(WEB_EXE) -O3 --shell-file index.html --preload-file ${ASSET_DIR}@
	@mv ${WEB_DIR}/index.data ../app/public/index.data

$(OBJ_DIR)/%.o: %.cpp
	@printf "Compiling %s -> %s\n" "$<" "$@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@
	
$(WEB_DIR):
	@mkdir -p $@

scripting:
	@cd script-core/src && dotnet build
	@cp script-core/src/bin/Debug/netstandard2.0/ScriptCore.dll script-core/ScriptCore.dll

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
