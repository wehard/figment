#pragma once

#include "renderer.h"
#include <string>

namespace figment
{
enum class AppEvent
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved
};

class Layer
{
public:
    struct Context
    {
        figment::vulkan::Renderer* renderer;
    };
    explicit Layer(const std::string& name = "Layer", bool enabled = true):
        m_Name(name), m_Enabled(enabled){};
    virtual ~Layer()                                      = default;
    virtual void OnAttach(const Context& context)         = 0;
    virtual void OnDetach()                               = 0;
    virtual void OnEnable()                               = 0;
    virtual void OnDisable()                              = 0;
    virtual void OnUpdate(float deltaTime)                = 0;
    virtual void OnImGuiRender()                          = 0;
    virtual void OnEvent(AppEvent event, void* eventData) = 0;
    void SetEnabled(bool enabled)
    {
        m_Enabled = enabled;
        if (m_Enabled)
            OnEnable();
        else
            OnDisable();
    }
    [[nodiscard]] bool IsEnabled() const { return m_Enabled; }
    [[nodiscard]] std::string GetName() const { return m_Name; }

protected:
    std::string m_Name;
    bool m_Enabled = true;

private:
    friend class App;
    friend class Application;
};
} // namespace figment
