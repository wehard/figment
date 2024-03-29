#pragma once

#include "Figment.h"

using namespace Figment;

class GameOfLife : public Figment::Layer
{
public:
    constexpr static uint32_t s_DeadColor = 0xffffb273;
    constexpr static uint32_t s_LiveColor = 0xff0000ff;

public:
    GameOfLife(WebGPUContext &context, PerspectiveCamera &camera);
    ~GameOfLife() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;
private:
    struct TextureData
    {
        uint32_t Width;
        uint32_t Height;
    };

    WebGPUContext &m_Context;
    PerspectiveCamera &m_Camera;
    PixelCanvas *m_PixelCanvas;
    WebGPUUniformBuffer<TextureData> *m_UniformBuffer;
    uint32_t *m_PrevPixelData;

    constexpr static uint32_t s_Width = 320;
    constexpr static uint32_t s_Height = 200;
};
