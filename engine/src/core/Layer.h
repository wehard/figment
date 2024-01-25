#pragma once

#include <string>

namespace Figment
{
    enum class AppEvent
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved
    };

    class Layer
    {
    public:
        friend class App;
        explicit Layer(const std::string &name = "Layer") : m_Name(name)
        { };
        virtual ~Layer() = default;
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnImGuiRender() = 0;
        virtual void OnEvent(AppEvent event, void *eventData) = 0;
        void SetEnabled(bool enabled)
        { m_Enabled = enabled; }
    protected:
        std::string m_Name;
        bool m_Enabled = true;
    };
}
