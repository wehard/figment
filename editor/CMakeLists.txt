cmake_minimum_required(VERSION 3.12)
project(editor)

add_executable(editor)

target_compile_definitions(editor PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(editor PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(editor PRIVATE -Wno-deprecated-literal-operator)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wno-dangling-else")
endif ()

add_custom_target(setup_emscripten_env
        COMMAND source ~/dev/emsdk/emsdk_env.sh
)

add_dependencies(editor setup_emscripten_env)

target_include_directories(editor
        PRIVATE
        src
        ../engine/src
        ../engine/src/core
        ../engine/src/scene
        ../engine/src/scene/components
        ../engine/src/physics
        ../engine/src/renderer
        ../engine/src/platform/webgpu
        ../engine/lib
        ../engine/lib/entt
        ../engine/lib/imgui/include
        ../engine/lib/glm
)

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(editor PRIVATE ${SRC_FILES})

set_target_properties(editor PROPERTIES LINK_FLAGS "\
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
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/../engine/index.html\
")

# link static engine lib
target_link_directories(editor PRIVATE ../engine/build)
target_link_libraries(editor libengine.a)

add_custom_target(cleanup
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.data
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.wasm
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.js
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/index.html
        COMMENT "Removed ems files."
)

add_dependencies(editor cleanup)

add_custom_command(
        TARGET editor
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
        COMMENT "Copied resources to ${CMAKE_BINARY_DIR}"
)
#
add_custom_command(TARGET editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html
        COMMENT "Copied shell file to: ${CMAKE_CURRENT_BINARY_DIR}"
)
