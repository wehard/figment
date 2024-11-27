#pragma once

#include <cstdint>

namespace figment
{
class FPSCounter
{
public:
    FPSCounter();
    ~FPSCounter() = default;

    void Update(float deltaTime);
    float GetFPS() const { return m_FPS; }
    float GetFrameTime() const { return m_FrameTime; }
    uint32_t GetFrameCount() const { return m_TotalFrameCount; }

private:
    float m_FPS;
    float m_FrameTime;
    float m_TimeAccumulator;
    uint32_t m_FrameCount;
    uint32_t m_TotalFrameCount;
};
} // namespace figment
