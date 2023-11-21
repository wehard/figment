#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <memory>

namespace Figment
{
    class GfxContext
    {
    public:
        virtual ~GfxContext() = default;
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<GfxContext> Create(void *window);
    };
}
