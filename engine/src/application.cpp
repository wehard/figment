#include "application.h"
#include "Input.h"
#include "RenderStats.h"

#include <BaseWindow.h>
#include <debug_tools.h>
#include <image_transition.h>
#include <spdlog/spdlog.h>

namespace figment
{
FPSCounter Application::FPScounter;

Application::Application(const Descriptor&& descriptor):
    window(descriptor.Name, descriptor.Width, descriptor.Height),
    renderer(*window.GetContext<vulkan::Context>()),
    imguiRenderer({
        .window         = window.GetNative(),
        .instance       = window.GetContext<vulkan::Context>()->GetInstance(),
        .device         = window.GetContext<vulkan::Context>()->GetDevice(),
        .physicalDevice = window.GetContext<vulkan::Context>()->GetPhysicalDevice(),
        .queue          = window.GetContext<vulkan::Context>()->GetGraphicsQueue(),
        .descriptorPool = window.GetContext<vulkan::Context>()->createDescriptorPool(
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 256},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 256},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 256},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 256},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 256},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 256},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 256},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 256},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 256},
            },
            256),
        .minImageCount   = window.surfaceCapabilities().minImageCount,
        .swapchainFormat = window.surfaceFormat.format,
    })
{
    Input::Initialize(window.GetNative());

    window.SetResizeEventCallback(
        [this](Window::ResizeEventData eventData)
        {
            for (auto layer: m_LayerStack)
            {
                if (!layer->m_Enabled)
                    continue;
                layer->OnEvent(AppEvent::WindowResize, (void*)&eventData);
            }
        });
    spdlog::info("Application created");
}

void Application::update()
{
    Input::Update();
    RenderStats::Reset();

    m_CurrentTime   = (float)glfwGetTime();
    float deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime      = m_CurrentTime;
    m_TimeSinceStart += deltaTime;

    FPScounter.Update(deltaTime);

    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnUpdate(deltaTime);
    }

    const auto renderData = renderer.begin(window.swapchain());

    imguiRenderer.begin(
        renderData.commandBuffer,
        {
            .image      = renderData.image,
            .imageView  = renderData.imageView,
            .renderArea = {.offset = {0, 0}, .extent = {window.GetWidth(), window.GetHeight()}},
        });
    for (const auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnImGuiRender();
    }
    imguiRenderer.end(renderData.commandBuffer);

    renderer.end(window.swapchain());

    glfwPollEvents();
}

void Application::addLayer(Layer* layer)
{
    spdlog::info("Adding layer: {}", layer->GetName());
    m_LayerStack.AddLayer(layer);
    layer->OnAttach();
}

void Application::addOverlay(Layer* overlay)
{
    spdlog::info("Adding overlay: {}", overlay->GetName());
    m_LayerStack.AddOverlay(overlay);
    overlay->OnAttach();
}

void Application::start()
{
    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnEnable();
    }

    while (!window.ShouldClose() && !Input::GetKey(GLFW_KEY_ESCAPE))
    {
        update();
    }

    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnDisable();
    }
    vkQueueWaitIdle(window.GetContext<vulkan::Context>()->GetGraphicsQueue());
}
} // namespace figment
