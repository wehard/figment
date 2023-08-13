cmake_minimum_required(VERSION 3.12)
project(figment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(
        src
        lib/imgui/include
        lib/glm
        lib
        lib/entt
        src/scene
        src/physics
        src/platform/opengl
        src/scripting
        lib/mono/include
        /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native
        /opt/homebrew/include
)

link_directories(
        /opt/homebrew/lib
        /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native
        /opt/homebrew/opt/zlib/lib
        lib/mono/lib
)

file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
        "lib/imgui/*.cpp"
)

add_executable(figment "")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mono DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/script-core DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE EXCLUDED_SRC_FILES
        "src/platform/web/*.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(figment PRIVATE ${SRC_FILES})

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(figment
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        -lglfw
        -lGLEW
        -lm
        -lnethost
        -lz
        -liconv
        -lmonosgen-2.0
)

add_custom_command(TARGET figment POST_BUILD
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/script-core/src && dotnet build
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/script-core/src/bin/Debug/netstandard2.0/ScriptCore.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/script-core/ScriptCore.dll
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

#add_executable(figment_wasm "")
#
#file(GLOB_RECURSE EXCLUDED_SRC_FILES
#        "src/platform/macos/*.cpp"
#        "src/scripting"
#)
#
#list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})
#
#target_sources(figment_wasm PRIVATE ${SRC_FILES})