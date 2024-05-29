cmake_minimum_required(VERSION 3.12)
project(vk-app)

add_executable(vk-app)
target_compile_definitions(vk-app PRIVATE VULKAN_BACKEND)
set_property(TARGET vk-app PROPERTY CXX_STANDARD 20)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME} for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug symbols enabled")
    target_compile_definitions(vk-app PRIVATE DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    message(STATUS "Optimization level: -O0")
endif ()

target_include_directories(vk-app
        PRIVATE
        src
        ../engine/src
        ../engine/src/core
        ../engine/src/scene
        ../engine/src/debug
        ../engine/src/scene/components
        ../engine/src/physics
        ../engine/src/renderer
        ../engine/src/platform/vulkan
        ../engine/lib
        ../engine/lib/entt
        ../engine/lib/imgui
        ../engine/lib/imgui/backends
        ../engine/lib/glm
)

set(VULKAN_SDK "~/dev/vulkansdk/1.3.283.0/macOS")
target_include_directories(vk-app PRIVATE ${VULKAN_SDK}/include)
target_link_libraries(vk-app "/Users/willehard/dev/vulkansdk/1.3.283.0/macOS/lib/libvulkan.dylib")


target_include_directories(vk-app PRIVATE "/opt/homebrew/Cellar/glfw/3.4/include")
target_link_libraries(vk-app "/opt/homebrew/Cellar/glfw/3.4/lib/libglfw.dylib")

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
        "../engine/src/platform/vulkan/*.cpp"
)

target_sources(vk-app PRIVATE ${SRC_FILES})
target_sources(vk-app PRIVATE ../engine/src/core/Window.cpp)
target_sources(vk-app PRIVATE ../engine/src/core/Log.cpp)

target_sources(vk-app PRIVATE
        "../engine/lib/imgui/backends/imgui_impl_glfw.cpp"
        "../engine/lib/imgui/backends/imgui_impl_vulkan.cpp"
)

target_sources(vk-app PRIVATE
        "../engine/lib/imgui/imgui.cpp"
        "../engine/lib/imgui/imgui_draw.cpp"
        "../engine/lib/imgui/imgui_widgets.cpp"
        "../engine/lib/imgui/imgui_tables.cpp"
)


target_link_directories(vk-app PRIVATE ${CMAKE_BINARY_DIR}/engine)
#target_link_libraries(vk-app libengine.a)

add_custom_command(
        TARGET vk-app
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
        COMMENT "Copied resources to ${CMAKE_BINARY_DIR}"
)

add_custom_target(compile_shaders
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/res/shader.vert -o ${CMAKE_BINARY_DIR}/res/shader.vert.spv -V
        COMMAND glslangValidator ${CMAKE_SOURCE_DIR}/res/shader.frag -o ${CMAKE_BINARY_DIR}/res/shader.frag.spv -V
        COMMENT "Compiling shaders")

add_dependencies(vk-app compile_shaders)
