cmake_minimum_required(VERSION 3.12)
project(vk-app)

add_executable(vk-app)
target_compile_definitions(vk-app PRIVATE VULKAN_BACKEND)
set_property(TARGET vk-app PROPERTY CXX_STANDARD 20)
#add_subdirectory(../engine engine)

#add_dependencies(vk-app engine)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME} for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

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


target_include_directories(vk-app PRIVATE "/opt/homebrew/Cellar/glfw/3.3.8/include")
target_link_libraries(vk-app "/opt/homebrew/Cellar/glfw/3.3.8/lib/libglfw.3.3.dylib")

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
)

target_sources(vk-app PRIVATE ${SRC_FILES})
target_sources(vk-app PRIVATE ../engine/src/core/Window.cpp)
target_sources(vk-app PRIVATE ../engine/src/core/Log.cpp)
target_sources(vk-app PRIVATE
        "../engine/src/platform/vulkan/VulkanWindow.cpp"
        "../engine/src/platform/vulkan/VulkanContext.cpp"
        "../engine/src/platform/vulkan/VulkanBuffer.cpp")

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
