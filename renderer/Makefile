NAME = figment

SRCDIR = src
IMGUIDIR = lib/imgui

# Define the directories where source files are located
SRC_DIRS := $(shell find src lib/imgui -type d | grep -v "src/platform/web")

# Define the directory where object files will be placed
OBJ_DIR := build/objs

# Get a list of all the source files in the source directories
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Generate a list of object files to build
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

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

UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -p)

CC = clang++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "Building \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@printf "Compiling %s -> %s\n" "$<" "$@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@

scripting:
	@cd script-core/src && dotnet build
	@cp script-core/src/bin/Debug/netstandard2.0/ScriptCore.dll script-core/ScriptCore.dll

debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	@rm -rf $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -rf build

re: fclean all

.PHONY: all scripting clean fclean re debug
