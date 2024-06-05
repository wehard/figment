cmake_minimum_required(VERSION 3.12)
project(portfolio)

add_subdirectory(../engine engine)

add_executable(portfolio)

add_dependencies(portfolio engine)

target_compile_definitions(portfolio PRIVATE WEBGPU_BACKEND)
target_compile_definitions(portfolio PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(portfolio PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(portfolio PRIVATE -Wno-deprecated-literal-operator)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME} for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wno-dangling-else")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif ()

target_include_directories(portfolio
        PRIVATE
        src/Asteroids
        src/Cube
        src/Galaxy
        src/GameOfLife
        src/MetaGameSim
        src/Shapes
        src/Worlds
        ../engine/src
        ../engine/src/core
        ../engine/src/scene
        ../engine/src/debug
        ../engine/src/scene/components
        ../engine/src/physics
        ../engine/src/renderer
        ../engine/src/platform/webgpu
        ../engine/lib
        ../engine/lib/entt
        ../engine/lib/imgui
        ../engine/lib/imgui/include
        ../engine/lib/glm
)

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
)

target_sources(portfolio PRIVATE ${SRC_FILES})

set_target_properties(portfolio PROPERTIES LINK_FLAGS "\
        -s USE_GLFW=3\
        -s DISABLE_EXCEPTION_CATCHING=0\
        -s WASM=1\
        -s USE_WEBGPU=1\
        -s ALLOW_MEMORY_GROWTH=1\
        -s NO_EXIT_RUNTIME=0\
        -s ASSERTIONS=1\
        -s USE_ES6_IMPORT_META=1\
        --preload-file res@/res\
        --preload-file ../../../engine/res/shaders@/res/shaders/builtin\
        -lglfw\
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html\
")

target_link_directories(portfolio PRIVATE ${CMAKE_BINARY_DIR}/engine)
target_link_libraries(portfolio libengine.a)

add_custom_target(cleanup
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/portfolio.data
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/portfolio.wasm
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/portfolio.js
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/index.html
        COMMENT "Removed ems files."
)

add_dependencies(portfolio cleanup)

add_custom_command(
        TARGET portfolio
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
        COMMENT "Copied resources to ${CMAKE_BINARY_DIR}"
)

add_custom_command(TARGET portfolio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html
        COMMENT "Copied shell file to: ${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_command(TARGET portfolio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/app.yaml ${CMAKE_CURRENT_SOURCE_DIR}/dist/app.yaml
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/index.html ${CMAKE_CURRENT_SOURCE_DIR}/dist/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/portfolio.js ${CMAKE_CURRENT_SOURCE_DIR}/dist/portfolio.js
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/portfolio.data ${CMAKE_CURRENT_SOURCE_DIR}/dist/portfolio.data
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/portfolio.wasm ${CMAKE_CURRENT_SOURCE_DIR}/dist/portfolio.wasm
        COMMENT "Copied files to dist folder"
)
