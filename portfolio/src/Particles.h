#pragma once

#include "Figment.h"

using namespace Figment;

struct Particle
{
    glm::vec3 Position;
    uint32_t _Padding[1];
};

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
    SharedPtr<WebGPUContext> m_Context;
    SharedPtr<PerspectiveCamera> m_Camera;
    UniquePtr<WebGPURenderer> m_Renderer;
    UniquePtr<WebGPUShader> m_Shader;
    UniquePtr<WebGPUShader> m_ParticleShader;
    UniquePtr<WebGPUVertexBuffer<Particle>> m_VertexBuffer;
};
