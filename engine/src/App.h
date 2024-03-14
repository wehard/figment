#pragma once

#include "Entity.h"
#include "Framebuffer.h"
#include "Window.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Scene.h"
#include "FPSCounter.h"
#include "imgui.h"
#include "ShapeRenderer.h"
#include "WebGPUGUIContext.h"
#include "LayerStack.h"
#include "Layer.h"

#include <memory>
#include <vector>

namespace Figment
{
    class App
    {
    public:
        App(uint32_t width, uint32_t height);
        ~App();

        void AddLayer(Layer *layer);
        void AddOverlay(Layer *overlay);
        void Update();
        std::shared_ptr<Window> GetWindow()
        { return m_Window; }
        FPSCounter &GetFPSCounter()
        { return m_FPSCounter; }
        float GetTimeSinceStart() const
        { return m_TimeSinceStart; }
        static App *Instance()
        { return s_Instance; }
    private:
        std::shared_ptr<Window> m_Window;
        std::unique_ptr<WebGPUGUIContext> m_GUICtx;
        LayerStack m_LayerStack;

        float m_CurrentTime = 0;
        float m_LastTime = 0;
        float m_TimeSinceStart = 0;

        FPSCounter m_FPSCounter;

        static App *s_Instance;
    };
}
