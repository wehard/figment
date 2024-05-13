cmake_minimum_required(VERSION 3.12)
project(editor)

add_subdirectory(../engine engine)

add_executable(editor)

set_property(TARGET editor PROPERTY CXX_STANDARD 20)

add_dependencies(editor engine)

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
        ../engine/lib/imgui/include
        ../engine/lib/glm
)

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
)

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
        -s 'EXPORTED_FUNCTIONS=[\"_free\", \"_main\"]'\
        --preload-file res@/res\
        --preload-file ../../../engine/res/shaders@/res/shaders/builtin\
        -lglfw\
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html\
        --js-library ${CMAKE_CURRENT_SOURCE_DIR}/figment_lib.js\
        --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/figment_api.js\
")

target_link_directories(editor PRIVATE ${CMAKE_BINARY_DIR}/engine)
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

add_custom_command(TARGET editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html
        COMMENT "Copied shell file to: ${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/figment_api.js ${CMAKE_CURRENT_BINARY_DIR}/figment_api.js
        COMMENT "Copied api js file to: ${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/figment_template.js ${CMAKE_CURRENT_BINARY_DIR}/figment_template.js
        COMMENT "Copied template js file to: ${CMAKE_CURRENT_BINARY_DIR}"
)

if (CMAKE_BUILD_TYPE STREQUAL "Test")
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
            DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
            figment_tests
            test/pool_tests.cpp
            test/handle_tests.cpp
    )

    set_property(TARGET figment_tests PROPERTY CXX_STANDARD 20)

    target_include_directories(figment_tests PRIVATE ../engine/src/core)
    target_link_libraries(
            figment_tests
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(figment_tests)
endif ()
