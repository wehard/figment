#pragma once

#include <string>

namespace Figment
{
    class Layer
    {
    public:
        explicit Layer(const std::string &name = "Layer") : m_Name(name) {};
        virtual ~Layer() = default;
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnImGuiRender() = 0;
    protected:
        std::string m_Name;
    };
}
