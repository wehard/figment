#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <memory>

namespace Figment
{
    class RenderContext
    {
    public:
        virtual ~RenderContext() = default;
        virtual void Init(uint32_t width, uint32_t height) = 0;
        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        static std::unique_ptr<RenderContext> Create(void *window);
    };
}
