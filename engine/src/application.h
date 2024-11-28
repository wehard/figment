#pragma once

#include "BaseWindow.h"
#include "FPSCounter.h"
#include "LayerStack.h"
#include "imgui_renderer.h"

#include <BaseWindow.h>
#include <cstdint>

namespace figment
{
class Application
{
public:
    struct Descriptor
    {
        const char* Name;
        uint32_t Width;
        uint32_t Height;
    };
    explicit Application(const Descriptor&& descriptor);
    void Start();
    void Update();
    void AddLayer(Layer* layer);
    void AddOverlay(Layer* overlay);

    [[nodiscard]] const Window& GetWindow() const { return window; }

    static FPSCounter FPScounter;

private:
    vulkan::Window window;
    vulkan::ImGuiRenderer imguiRenderer;
    LayerStack m_LayerStack;
    bool m_InputEnabled    = true;
    float m_CurrentTime    = 0;
    float m_LastTime       = 0;
    float m_TimeSinceStart = 0;
};

} // namespace figment
