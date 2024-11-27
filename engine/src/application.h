#pragma once

#include "FPSCounter.h"
#include "LayerStack.h"
#include "BaseWindow.h"
#include "imgui_renderer.h"

#include <cstdint>
#include <BaseWindow.h>

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

private:
    vulkan::Window window;
    vulkan::ImGuiRenderer imguiRenderer;
    LayerStack m_LayerStack;
    bool m_InputEnabled = true;
    FPSCounter m_FPSCounter;
    float m_CurrentTime    = 0;
    float m_LastTime       = 0;
    float m_TimeSinceStart = 0;
};

} // namespace figment
