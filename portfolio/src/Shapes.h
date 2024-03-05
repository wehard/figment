#pragma once

#include "Figment.h"

using namespace Figment;

class Shapes : public Layer
{
public:
    Shapes(WebGPUContext &context, PerspectiveCamera &camera);
    ~Shapes() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    ShapeRenderer m_ShapeRenderer;
    WebGPUContext &m_Context;
    PerspectiveCamera &m_Camera;
};
