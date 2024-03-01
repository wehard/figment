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
    PixelCanvas m_Canvas;
    PerspectiveCamera &m_Camera;
    PixelCanvas *m_PixelCanvas;
};
