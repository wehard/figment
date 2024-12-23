#pragma once

#include <cstdint>

namespace figment
{
class RenderStats
{
public:
    static uint32_t DrawCalls;
    static uint32_t VertexCount;
    static uint32_t CircleCount;
    static uint32_t QuadCount;
    static uint32_t ParticleCount;

    static void Reset()
    {
        DrawCalls     = 0;
        VertexCount   = 0;
        CircleCount   = 0;
        QuadCount     = 0;
        ParticleCount = 0;
    }
};
} // namespace figment
