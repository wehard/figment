cmake_minimum_required(VERSION 3.12)

option(USE_VULKAN "Build with Vulkan support" OFF)

project(engine)
add_library(engine STATIC)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)

message(STATUS "Configuring engine for ${CMAKE_BUILD_TYPE} (${CMAKE_SYSTEM_NAME})")

target_include_directories(engine PUBLIC
        src
        src/core
        src/renderer
        src/scene
        src/scene/components

        src/debug
        src/utils

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
)

find_package(glm CONFIG REQUIRED)
target_link_libraries(engine glm::glm)

if (EMSCRIPTEN)
    target_compile_definitions(engine PRIVATE WEBGPU_BACKEND)

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
    )
else ()
    target_compile_definitions(engine PRIVATE VULKAN_BACKEND)

    find_package(glfw3 CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(imgui CONFIG REQUIRED)

    target_link_libraries(engine
            glfw
            Vulkan::Vulkan
            imgui::imgui
    )

    target_include_directories(engine PUBLIC
            src/platform/vulkan
    )

    file(GLOB_RECURSE VULKAN_SRC_FILES
            src/platform/vulkan/*.cpp
    )
    target_sources(engine PRIVATE
            ${VULKAN_SRC_FILES}
            src/debug/DebugPanel.cpp
            src/scene/CameraController.cpp
            src/scene/Scene.cpp
            src/scene/Entity.cpp

    )
    add_subdirectory(test)
endif ()
