cmake_minimum_required(VERSION 3.12)
project(engine)

add_library(engine STATIC)

target_compile_definitions(engine PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(engine PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(engine PRIVATE -Wno-deprecated-literal-operator)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wno-dangling-else")
endif ()

add_custom_target(setup_emscripten_env
        COMMAND source ~/dev/emsdk/emsdk_env.sh
)

add_dependencies(engine setup_emscripten_env)

target_include_directories(engine
        PRIVATE
        src
        src/core
        src/scene
        src/scene/components
        src/physics
        src/platform/webgpu/
        lib
        lib/glm
        lib/entt
        lib/imgui/include
)

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
        "lib/imgui/*.cpp"
)

file(GLOB_RECURSE EXCLUDED_SRC_FILES
        "src/platform/macos/*.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(engine PRIVATE ${SRC_FILES})

set_target_properties(engine PROPERTIES LINK_FLAGS "\
        -s USE_GLFW=3\
        -s DISABLE_EXCEPTION_CATCHING=0\
        -s WASM=1\
        -s USE_WEBGPU=1\
        -s ALLOW_MEMORY_GROWTH=1\
        -s NO_EXIT_RUNTIME=0\
        -s ASSERTIONS=1\
        -s USE_ES6_IMPORT_META=1\
        --preload-file res@/res\
        -lGLEW\
        -lglfw\
        -g\
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html\
")
