cmake_minimum_required(VERSION 3.12)

option(USE_VULKAN "Build with Vulkan support" OFF)

project(engine)
add_library(engine STATIC)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

# Platform-specific configurations
if (EMSCRIPTEN)
    target_compile_definitions(engine PRIVATE WEBGPU_BACKEND)
    set(EMSCRIPTEN_PATH "~/dev/emsdk")
    add_custom_target(setup_emscripten
            COMMAND source ${EMSCRIPTEN_PATH}/emsdk_env.sh
    )
    add_dependencies(engine setup_emscripten)
    target_include_directories(engine PRIVATE src/platform/webgpu)

    target_sources(engine PRIVATE
            "lib/imgui/backends/imgui_impl_glfw.cpp"
            "lib/imgui/backends/imgui_impl_wgpu.cpp"
    )

    target_compile_definitions(engine PRIVATE GL_SILENCE_DEPRECATION)
    target_compile_options(engine PRIVATE -Wno-deprecated-literal-operator)
else ()
    set(VULKAN_SDK "~/dev/vulkansdk/1.3.283.0/macOS")
    target_compile_definitions(engine PRIVATE VULKAN_BACKEND)
    #    find_package(Vulkan REQUIRED)
    target_include_directories(engine PRIVATE ${VULKAN_SDK}/include)
    #    target_link_libraries(engine PRIVATE ${Vulkan_LIBRARIES})
    target_include_directories(engine PRIVATE "/opt/homebrew/Cellar/glfw/3.3.8/include")
    target_include_directories(engine PRIVATE src/platform/vulkan)

    target_sources(engine PRIVATE
            "lib/imgui/backends/imgui_impl_glfw.cpp"
            "lib/imgui/backends/imgui_impl_vulkan.cpp"
    )
endif ()

message(STATUS "Configuring engine for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

target_include_directories(engine
        PRIVATE
        src
        src/core
        src/renderer
        src/scene
        src/scene/components
        src/physics
        lib
        lib/glm
        lib/entt
        lib/imgui
        lib/imgui/backends
)

file(GLOB_RECURSE SRC_FILES
        "src/core/*.cpp"
        "src/debug/*.cpp"
        "src/physics/*.cpp"
        "src/renderer/*.cpp"
        "src/scene/*.cpp"
        "src/App.cpp"
        "PixelCanvas.cpp"
        "lib/imgui/imgui.cpp"
        "lib/imgui/imgui_draw.cpp"
        "lib/imgui/imgui_widgets.cpp"
        "lib/imgui/imgui_tables.cpp"
)

if (EMSCRIPTEN)
    file(GLOB_RECURSE WEBGPU_SRC_FILES
            "src/platform/webgpu/*.cpp"
    )
    target_sources(engine PRIVATE ${WEBGPU_SRC_FILES})
else ()
    file(GLOB_RECURSE VULKAN_SRC_FILES
            "src/platform/vulkan/*.cpp"
    )
    target_sources(engine PRIVATE ${VULKAN_SRC_FILES})
endif ()

target_sources(engine PRIVATE ${SRC_FILES})

if (NOT EMSCRIPTEN)
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
            src/core/ResourceManager.cpp
    )

    file(GLOB_RECURSE TEST_SRC_FILES
            "test/*.cpp"
    )

    target_sources(figment_tests PRIVATE
            ${TEST_SRC_FILES}
    )

    set_property(TARGET figment_tests PROPERTY CXX_STANDARD 20)

    target_include_directories(figment_tests PRIVATE ../engine/src/core ../engine/src/renderer)
    target_link_libraries(
            figment_tests
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(figment_tests)
endif ()
