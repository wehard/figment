cmake_minimum_required(VERSION 3.12)
project(editor)

add_executable(editor)

set_property(TARGET editor PROPERTY CXX_STANDARD 20)

target_link_libraries(editor engine)

target_compile_definitions(editor PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(editor PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(editor PRIVATE -Wno-deprecated-literal-operator)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME} for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wno-dangling-else")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 --closure 1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 --closure 1")
endif ()

target_include_directories(editor
        PRIVATE
        src
#        ../engine/src
#        ../engine/src/core
#        ../engine/src/scene
#        ../engine/src/debug
#        ../engine/src/scene/components
#        ../engine/src/physics
#        ../engine/src/renderer
#        ../engine/src/platform/webgpu
#        ../engine/lib
#        ../engine/lib/entt
#        ../engine/lib/imgui
#        ../engine/lib/imgui/backends
#        ../engine/lib/glm
)

target_sources(editor PRIVATE
        src/main.cpp
        src/EditorLayer.cpp
#        src/SceneRenderer.cpp
)

#set_target_properties(editor PROPERTIES LINK_FLAGS "\
#        -s USE_GLFW=3\
#        -s DISABLE_EXCEPTION_CATCHING=0\
#        -s WASM=1\
#        -s USE_WEBGPU=1\
#        -s ALLOW_MEMORY_GROWTH=1\
#        -s NO_EXIT_RUNTIME=0\
#        -s ASSERTIONS=1\
#        -s USE_ES6_IMPORT_META=1\
#        -s 'EXPORTED_FUNCTIONS=[\"_free\", \"_main\"]'\
#        --preload-file res@/res\
#        --preload-file ../../../engine/res/shaders@/res/shaders/builtin\
#        -lglfw\
#        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html\
#        --js-library ${CMAKE_CURRENT_SOURCE_DIR}/figment_lib.js\
#        --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/figment_api.js\
#")

#target_link_directories(editor PRIVATE ${CMAKE_BINARY_DIR}/engine)
#target_link_libraries(editor engine)

add_custom_target(cleanup
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.data
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.wasm
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/editor.js
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/index.html
        COMMENT "Removed ems files."
)

#add_dependencies(editor cleanup)

#add_custom_command(
#        TARGET editor
#        PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
#        COMMENT "Copied resources to ${CMAKE_CURRENT_BINARY_DIR}"
#)
#
#add_custom_command(TARGET editor POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html
#        COMMENT "Copied shell file to: ${CMAKE_CURRENT_BINARY_DIR}"
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/figment_api.js ${CMAKE_CURRENT_BINARY_DIR}/figment_api.js
#        COMMENT "Copied api js file to: ${CMAKE_CURRENT_BINARY_DIR}"
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/figment_template.js ${CMAKE_CURRENT_BINARY_DIR}/figment_template.js
#        COMMENT "Copied template js file to: ${CMAKE_CURRENT_BINARY_DIR}"
#)

file(GLOB_RECURSE SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/res/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/res/*.frag"
)

add_dependencies(editor shaders)

add_custom_target(shaders DEPENDS ${SHADER_FILES})

add_custom_command(
        TARGET shaders
        PRE_BUILD
        COMMAND for file in "${CMAKE_CURRENT_SOURCE_DIR}/res/*.vert" ";" do glslangValidator $\${file} -o $\${file}.spv -V --quiet ";" done
        COMMAND for file in "${CMAKE_CURRENT_SOURCE_DIR}/res/*.frag" ";" do glslangValidator $\${file} -o $\${file}.spv -V --quiet ";" done
        COMMENT "Compiling shaders"
)

add_custom_command(
        TARGET shaders
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
        COMMENT "Copied resources to ${CMAKE_CURRENT_BINARY_DIR}"
)
