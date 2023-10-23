#pragma once

#include "Entity.h"
#include "Framebuffer.h"
#include "Window.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Scene.h"
#include "imgui.h"
#include "WebGPURenderer.h"
#include "WebGPUGUIContext.h"
#include "Layer.h"

#include <memory>
#include <vector>

class App
{
public:
    App(float width, float height);
    ~App();

    void Update();
    void OnResize(uint32_t width, uint32_t height);
    std::shared_ptr<Window> GetWindow() { return m_Window; }

    static App *Instance() { return s_Instance; }
private:
    std::shared_ptr<Window> m_Window;
    std::unique_ptr<WebGPUGUIContext> m_GUICtx;
    std::vector<std::unique_ptr<Figment::Layer>> m_Layers;

    double m_CurrentTime = 0;
    double m_LastTime = 0;

    static App *s_Instance;
};
