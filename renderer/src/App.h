#pragma once

#include "Core.h"
#include "Entity.h"
#include "Framebuffer.h"
#include "Window.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Scene.h"
#include "FPSCounter.h"
#include "imgui.h"
#include "WebGPURenderer.h"
#include "WebGPUGUIContext.h"
#include "Layer.h"

#include <memory>
#include <vector>

class App
{
public:
    App(uint32_t width, uint32_t height);
    ~App();

    void Update();
    Figment::SharedPtr<Window> GetWindow() { return m_Window; }
    FPSCounter &GetFPSCounter() { return m_FPSCounter; }
    float GetTimeSinceStart() const { return m_TimeSinceStart; }
    static App *Instance() { return s_Instance; }
private:
    Figment::SharedPtr<Window> m_Window;
    Figment::UniquePtr<WebGPUGUIContext> m_GUICtx;
    std::vector<Figment::UniquePtr<Figment::Layer>> m_Layers;

    float m_CurrentTime = 0;
    float m_LastTime = 0;
    float m_TimeSinceStart = 0;

    FPSCounter m_FPSCounter;

    static App *s_Instance;
};
