#pragma once

#include "context.h"
#include "window.h"

#include <imgui.h>
#include <texture.h>

namespace figment::vulkan
{
class ImGuiRenderer
{
public:
    explicit ImGuiRenderer(const vulkan::Window& window);
    ~ImGuiRenderer();

    void beginFrame();
    void endFrame();

private:
    const Context& ctx;
    const Window& window;
    ImGuiContext* imguiContext    = nullptr;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    Texture depthAttachment       = {};
};

} // namespace figment::vulkan
