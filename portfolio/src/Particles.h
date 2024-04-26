#pragma once

#include "Figment.h"
#include "BindGroup.h"
#include "ComputePipeline.h"
#include "ParticleRenderer.h"

using namespace Figment;

struct Particle
{
    glm::vec3 Position;
    glm::vec3 PrevPosition;
    glm::vec3 Acceleration;
    uint32_t _Padding[3];
};

struct ParticlesData
{
    float DeltaTime;
    float Time;
    glm::vec2 Seed;
};

class Particles : public Layer
{
public:
    Particles(std::shared_ptr<PerspectiveCamera> camera, bool enabled);
    ~Particles() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
    float m_ParticleSize = 0.005f;
private:
    std::shared_ptr<WebGPUContext> m_Context;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<ParticleRenderer> m_Renderer;
    std::unique_ptr<WebGPUShader> m_ComputeShader;
    std::unique_ptr<WebGPUShader> m_ParticleShader;
    std::unique_ptr<WebGPUVertexBuffer<Particle>> m_VertexBuffer;
    std::unique_ptr<WebGPUUniformBuffer<ParticlesData>> m_UniformBuffer;
    BindGroup *m_ComputeBindGroup;
    ComputePipeline *m_InitPipeline;
    ComputePipeline *m_SimulatePipeline;
};
