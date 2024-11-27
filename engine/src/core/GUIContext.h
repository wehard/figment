#pragma once

#include "BaseWindow.h"

namespace Figment
{
    class GUIContext
    {
    public:
        virtual void Init(std::shared_ptr<Window> window, const char *glslVersion) = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
        virtual ~GUIContext() = default;

        static std::unique_ptr<GUIContext> Create();
    };
}
