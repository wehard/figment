cmake_minimum_required(VERSION 3.12)

option(USE_VULKAN "Build with Vulkan support" OFF)

project(engine)
add_library(engine STATIC)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

if (EMSCRIPTEN)

else ()

endif ()

message(STATUS "Configuring engine for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

target_include_directories(engine PUBLIC
        src
        src/core
        src/renderer
        src/scene
        src/scene/components

        src/debug

        lib
        lib/glm
        lib/imgui
        lib/imgui/backends
        lib/entt
)

target_sources(engine PRIVATE
        src/Application.cpp
        src/core/FPSCounter.cpp
        src/renderer/OrthographicCamera.cpp
        src/renderer/RenderStats.cpp
        src/renderer/PerspectiveCamera.cpp

        src/core/UUID.cpp
        src/core/Window.cpp
        src/core/Log.cpp
        src/core/Image.cpp
        src/core/Input.cpp
        src/core/LayerStack.cpp


        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
)

if (EMSCRIPTEN)
    target_compile_definitions(engine PRIVATE WEBGPU_BACKEND)
    set(EMSCRIPTEN_PATH "~/dev/emsdk")
    add_custom_target(setup_emscripten
            #            COMMAND source ${EMSCRIPTEN_PATH}/emsdk_env.sh
    )
    add_dependencies(engine setup_emscripten)

    target_include_directories(engine PUBLIC
            src/physics
            src/platform/webgpu
    )
    target_sources(engine PRIVATE
            src/App.cpp
            src/PixelCanvas.cpp
            src/scene/Scene.cpp
            src/scene/Entity.cpp
            src/scene/CameraController.cpp
            src/renderer/Mesh.cpp
            src/renderer/OverlayRenderer.cpp
            src/renderer/BindGroup.cpp
            src/renderer/RenderPipeline.cpp
            src/renderer/ShapeRenderer.cpp
            src/renderer/ComputePipeline.cpp
            src/renderer/ParticleRenderer.cpp
            src/renderer/ComputePass.cpp
            src/renderer/MeshRenderer.cpp
            src/physics/VerletPhysics.cpp
            src/debug/DebugPanel.cpp


            src/platform/webgpu/WebGPUContext.cpp
            src/platform/webgpu/WebGPUFramebuffer.cpp
            src/platform/webgpu/WebGPUGUIContext.cpp
            src/platform/webgpu/WebGPURenderPipeline.cpp
            src/platform/webgpu/WebGPUShader.cpp
            src/platform/webgpu/WebGPUTexture.cpp
            src/platform/webgpu/WebGPUWindow.cpp

            lib/imgui/backends/imgui_impl_wgpu.cpp
    )
else ()
    target_compile_definitions(engine PRIVATE VULKAN_BACKEND)

    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(engine glfw)

    find_package(Vulkan REQUIRED)
    target_link_libraries(engine Vulkan::Vulkan)

    target_include_directories(engine PUBLIC
            src/platform/vulkan
    )

    file(GLOB_RECURSE VULKAN_SRC_FILES
            src/platform/vulkan/*.cpp
    )
    target_sources(engine PRIVATE
            ${VULKAN_SRC_FILES}
            lib/imgui/backends/imgui_impl_vulkan.cpp
    )
endif ()

#if (NOT EMSCRIPTEN)
#    include(FetchContent)
#    FetchContent_Declare(
#            googletest
#            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#            DOWNLOAD_EXTRACT_TIMESTAMP true
#    )
#    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#    FetchContent_MakeAvailable(googletest)
#
#    enable_testing()
#
#    add_executable(
#            figment_tests
#    )
#
#    file(GLOB_RECURSE TEST_SRC_FILES
#            "test/*.cpp"
#    )
#
#    list(REMOVE_ITEM TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/ResourceManagerTests.cpp)
#
#    target_sources(figment_tests PRIVATE
#            ${TEST_SRC_FILES}
#    )
#
#    set_property(TARGET figment_tests PROPERTY CXX_STANDARD 20)
#
#    target_include_directories(figment_tests PRIVATE
#            ../engine/src/core
#            ../engine/src/renderer
#            ../engine/src/utils
#            ../engine/lib/glm
#    )
#    target_link_libraries(
#            figment_tests
#            GTest::gtest_main
#    )
#
#    include(GoogleTest)
#    gtest_discover_tests(figment_tests)
#endif ()

#add_subdirectory(test)
