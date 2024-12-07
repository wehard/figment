#pragma once

#include <cstdint>

namespace figment
{
class RenderContext
{
public:
    virtual ~RenderContext()                               = default;
    virtual void Init(uint32_t width, uint32_t height)     = 0;
    virtual void onResize(uint32_t width, uint32_t height) = 0;

    // static std::unique_ptr<RenderContext> Create(void *window);
};
} // namespace figment
