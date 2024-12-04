cmake_minimum_required(VERSION 3.12)

project(vk_imgui_presenter)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC
        imgui
)

target_sources(${PROJECT_NAME} PRIVATE
        vk_imgui_presenter.cpp
        imgui_renderer.cpp
)

target_link_libraries(${PROJECT_NAME}
        imgui::imgui
)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(spdlog CONFIG REQUIRED)
