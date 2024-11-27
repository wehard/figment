#pragma once

#include "Entity.h"
#include "Framebuffer.h"
#include "BaseWindow.h"
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
        void EnableInput()
        { m_InputEnabled = true; }
        void DisableInput()
        { m_InputEnabled = false; }
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
        std::shared_ptr<WebGPUContext> m_RenderContext;
        std::unique_ptr<WebGPUGUIContext> m_GUICtx;
        LayerStack m_LayerStack;

        bool m_InputEnabled = true;
        float m_CurrentTime = 0;
        float m_LastTime = 0;
        float m_TimeSinceStart = 0;

        FPSCounter m_FPSCounter;

        static App *s_Instance;
    };
}
