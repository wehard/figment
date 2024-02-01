#pragma once
#include "Figment.h"
#include "Particles.h"

using namespace Figment;

class WorldMap : public Layer
{
public:
    WorldMap(SharedPtr<PerspectiveCamera> camera, bool enabled);
    ~WorldMap() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    SharedPtr<WebGPUContext> m_Context;
    SharedPtr<PerspectiveCamera> m_Camera;
    UniquePtr<WebGPURenderer> m_Renderer;
    UniquePtr<WebGPUShader> m_ComputeShader;
    UniquePtr<WebGPUShader> m_ParticleShader;
    UniquePtr<WebGPUVertexBuffer<Particle>> m_VertexBuffer;
    UniquePtr<WebGPUUniformBuffer<ParticlesData>> m_UniformBuffer;
    WebGPUTexture *m_WorldTexture;
};
