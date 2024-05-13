cmake_minimum_required(VERSION 3.12)

project(engine)
add_library(engine STATIC)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

target_compile_definitions(engine PRIVATE GL_SILENCE_DEPRECATION)
target_compile_definitions(engine PRIVATE WEBGPU_BACKEND_EMSCRIPTEN)
target_compile_options(engine PRIVATE -Wno-deprecated-literal-operator)

set(EMSCRIPTEN_PATH "~/dev/emsdk")

message(STATUS "Configuring engine for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

add_custom_target(setup_emscripten
        COMMAND source ${EMSCRIPTEN_PATH}/emsdk_env.sh
)

add_dependencies(engine setup_emscripten)

target_include_directories(engine
        PRIVATE
        src
        src/core
        src/renderer
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

target_sources(engine PRIVATE ${SRC_FILES})

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
