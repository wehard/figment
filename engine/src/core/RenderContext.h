#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <memory>

namespace Figment
{
    class RenderContext
    {
    public:
        virtual ~RenderContext() = default;
        virtual void Init() = 0;

        static std::unique_ptr<RenderContext> Create(void *window);
    };
}
