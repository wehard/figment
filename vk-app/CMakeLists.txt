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

add_subdirectory(/Users/willehard/dev/vk_debug_tools ${CMAKE_BINARY_DIR}/vk_debug_tools)
target_link_libraries(vk-app engine vk_debug_tools)

target_sources(vk-app PRIVATE
        src/main.cpp
)

file(GLOB_RECURSE SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/res/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/res/*.frag"
)

add_dependencies(vk-app shaders)

#add_custom_target(shaders DEPENDS ${SHADER_FILES})
#
#add_custom_command(
#        TARGET shaders
#        PRE_BUILD
#        COMMAND for file in "${CMAKE_CURRENT_SOURCE_DIR}/res/*.vert" ";" do glslangValidator $\${file} -o $\${file}.spv -V --quiet ";" done
#        COMMAND for file in "${CMAKE_CURRENT_SOURCE_DIR}/res/*.frag" ";" do glslangValidator $\${file} -o $\${file}.spv -V --quiet ";" done
#        COMMENT "Compiling shaders"
#)
#
#add_custom_command(
#        TARGET shaders
#        PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
#        COMMENT "Copied resources to ${CMAKE_CURRENT_BINARY_DIR}"
#)
