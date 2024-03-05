#pragma once

#include "Figment.h"

using namespace Figment;

class SandSimulation : public Figment::Layer
{
public:
    SandSimulation(WebGPUContext &context, PerspectiveCamera &camera);
    ~SandSimulation() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;
private:
    WebGPUContext &m_Context;
    PerspectiveCamera &m_Camera;
    PixelCanvas *m_PixelCanvas;

    bool CanMove(PixelCanvas &canvas, int x, int y);
    void UpdateSand(int x, int y);
    void UpdateWater(int x, int y);
    bool m_Dirty = false;

    constexpr static uint32_t s_Width = 320;
    constexpr static uint32_t s_Height = 200;
    constexpr static uint32_t s_SandColor = 0xff004b96;
    constexpr static uint32_t s_WaterColor = 0xffff0000;
    constexpr static uint32_t s_BackgroundColor = 0xffa89ba1;
};
