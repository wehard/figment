#pragma once

#include "Figment.h"

using namespace Figment;

class Shapes : public Layer
{
public:
    Shapes(WebGPUContext &context);
    ~Shapes() override = default;
    void OnAttach() override;
    void OnDetach() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    ShapeRenderer m_ShapeRenderer;
    WebGPUContext &m_Context;
    OrthographicCamera m_Camera;

    std::vector<glm::vec3> m_Points;
};
