#pragma once

#include "Figment.h"

using namespace Figment;

class Particles : public Layer
{
public:
    Particles(SharedPtr<PerspectiveCamera> camera);
    ~Particles() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    SharedPtr<PerspectiveCamera> m_Camera;
    UniquePtr<WebGPURenderer> m_Renderer;
};
