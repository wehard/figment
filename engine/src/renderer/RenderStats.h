#pragma once

#include <cstdint>

namespace Figment
{
    class RenderStats
    {
    public:
        static uint32_t DrawCalls;
        static uint32_t VertexCount;
        static uint32_t CircleCount;
        static uint32_t QuadCount;

        static void Reset()
        {
            DrawCalls = 0;
            VertexCount = 0;
            CircleCount = 0;
            QuadCount = 0;
        }
    };
}
