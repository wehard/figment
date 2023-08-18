#pragma once

#include "GfxContext.h"

class WebGPUContext : public GfxContext
{
public:
    void Init() override;
    void SwapBuffers() override;
};
