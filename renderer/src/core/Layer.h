#pragma once

namespace Figment
{
    class Layer
    {
    public:
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnImGuiRender() = 0;
    };
}
