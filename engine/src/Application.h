#pragma once

#include <cstdint>
#include "Window.h"
#include "LayerStack.h"
#include "FPSCounter.h"

namespace Figment
{
    class Application
    {
    public:
        struct Descriptor
        {
            const char *Name;
            uint32_t Width;
            uint32_t Height;
        };
        explicit Application(const Descriptor &&descriptor);
        void Update();
        void AddLayer(Layer *layer);
        void AddOverlay(Layer *overlay);
    private:
        Window m_Window;
        LayerStack m_LayerStack;
        bool m_InputEnabled = true;
        FPSCounter m_FPSCounter;
        float m_CurrentTime = 0;
        float m_LastTime = 0;
        float m_TimeSinceStart = 0;
    };

}
