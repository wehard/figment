#include "FPSCounter.h"

namespace Figment
{
    FPSCounter::FPSCounter()
            : m_FPS(0.0f), m_FrameTime(0.0f), m_TimeAccumulator(0.0f), m_FrameCount(0), m_TotalFrameCount(0)
    {
    }

    void FPSCounter::Update(float deltaTime)
    {
        m_TimeAccumulator += deltaTime;
        m_FrameCount++;
        m_TotalFrameCount++;

        if (m_TimeAccumulator >= 1.0f)
        {
            m_FPS = (float)m_FrameCount / m_TimeAccumulator;
            m_FrameTime = 1000.0f / m_FPS;
            m_TimeAccumulator = 0.0f;
            m_FrameCount = 0;
        }
    }
}
