cmake_minimum_required(VERSION 3.12)
project(engine)

add_executable(figment_wasm "")

target_compile_definitions(figment_wasm PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(figment_wasm PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(figment_wasm PRIVATE -Wno-deprecated-literal-operator)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wno-dangling-else")
endif ()

add_custom_target(setup_emscripten_env
        COMMAND source ~/dev/emsdk/emsdk_env.sh
)

add_dependencies(figment_wasm setup_emscripten_env)

target_include_directories(figment_wasm
        PRIVATE
        src
        src/core
        src/scene
        src/scene/components
        src/physics
        src/platform/opengl
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
        "src/platform/opengl/*.cpp"
        "src/scripting/*.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(figment_wasm PRIVATE ${SRC_FILES})

set_target_properties(figment_wasm PROPERTIES LINK_FLAGS "\
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

add_custom_target(cleanup
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/figment_wasm.data
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/figment_wasm.wasm
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/figment_wasm.js
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/index.html
        COMMENT "Removed ems files."
)

add_dependencies(figment_wasm cleanup)

add_custom_command(
        TARGET figment_wasm
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/mono ${CMAKE_BINARY_DIR}/mono
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/script-core ${CMAKE_BINARY_DIR}/script-core
        COMMENT "Copied resources to ${CMAKE_BINARY_DIR}"
)

add_custom_command(TARGET figment_wasm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html
        COMMENT "Copied shell file to: ${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_command(TARGET figment_wasm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.wasm ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/renderer/figment_wasm.wasm
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.js ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/renderer/figment_wasm.js
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.data ${CMAKE_CURRENT_SOURCE_DIR}/../app/public/figment_wasm.data
        COMMENT "Copied build files to app."
)


