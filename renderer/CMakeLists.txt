cmake_minimum_required(VERSION 3.12)
project(figment)

set(CMAKE_CXX_STANDARD 17)

add_executable(figment "")

target_include_directories(figment PRIVATE
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
target_link_directories(figment PRIVATE
        /opt/homebrew/lib
        /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/7.0.8/runtimes/osx-arm64/native
        /opt/homebrew/opt/zlib/lib
        lib/mono/lib
)


# Add the GL_SILENCE_DEPRECATION preprocessor definition to the target
target_compile_definitions(figment PRIVATE GL_SILENCE_DEPRECATION)

#add_custom_command(
#        TARGET figment
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/mono ${CMAKE_BINARY_DIR}/mono
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/script-core ${CMAKE_BINARY_DIR}/script-core
#)

        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mono DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/script-core DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
        "lib/imgui/*.cpp"
)

file(GLOB_RECURSE EXCLUDED_SRC_FILES
        "src/platform/webgpu/*.cpp"
        "lib/imgui/imgui_impl_wgpu.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(figment PRIVATE ${SRC_FILES})

#find_package(glfw3 REQUIRED)
#find_package(GLEW REQUIRED)

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

# Add a custom target to build the .NET project
add_custom_target(build_scriptcore
        COMMAND dotnet build > /dev/null
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/script-core/src
        COMMENT "Building ScriptCore"
)

# Copy the compiled DLL after building the .NET project
add_custom_command(TARGET build_scriptcore POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/script-core/src/bin/Debug/netstandard2.0/ScriptCore.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/script-core/ScriptCore.dll
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link the custom target to the main executable
add_dependencies(figment build_scriptcore)

### Figment WASM


add_executable(figment_wasm "")

target_compile_definitions(figment_wasm PRIVATE GL_SILENCE_DEPRECATION)

add_custom_target(setup_emscripten_env
        COMMAND source ~/dev/emsdk/emsdk_env.sh
)

add_dependencies(figment_wasm setup_emscripten_env)

target_include_directories(figment_wasm
        PRIVATE
        src
        src/scene
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
        "src/scripting/*.cpp"
)

list(REMOVE_ITEM SRC_FILES ${EXCLUDED_SRC_FILES})

target_sources(figment_wasm PRIVATE ${SRC_FILES})

set_target_properties(figment_wasm PROPERTIES LINK_FLAGS "\
        -s USE_GLFW=3\
        -s DISABLE_EXCEPTION_CATCHING=0\
        -s EXPORTED_FUNCTIONS=_onMouseMove,_insertObject,_onCanvasResize,_main\
        -s EXPORTED_RUNTIME_METHODS='[\"cwrap\", \"allocate\", \"intArrayFromString\", \"ALLOC_NORMAL\", \"UTF8ToString\"]'\
        -s MODULARIZE=1\
        -s EXPORT_ES6=1\
        -s WASM=1\
        -s USE_WEBGPU=1\
        -s ALLOW_MEMORY_GROWTH=1\
        -s NO_EXIT_RUNTIME=0\
        -s ASSERTIONS=1\
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html\
        --preload-file res@/res\
        -lGLEW\
        -lglfw\
")

add_custom_command(TARGET figment_wasm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.wasm ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/renderer/figment_wasm.wasm
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.js ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/renderer/figment_wasm.js
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/figment_wasm.data ${CMAKE_CURRENT_SOURCE_DIR}/../app/public/figment_wasm.data
)