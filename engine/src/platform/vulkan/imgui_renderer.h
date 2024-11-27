#pragma once

#include "context.h"

#include <imgui.h>
#include <texture.h>
#include <window.h>

namespace figment::vulkan
{

class ImGuiRenderer
{
public:
    explicit ImGuiRenderer(const Window& window);
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
