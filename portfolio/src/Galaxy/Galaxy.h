#pragma once

#include "Figment.h"
#include "BindGroup.h"
#include "ComputePipeline.h"
#include "ParticleRenderer.h"

using namespace Figment;

struct Particle
{
    glm::vec3 Position;
    uint32_t _Padding[1];
    glm::vec4 Color;
};

struct ParticlesData
{
    float DeltaTime;
    float Time;
    glm::vec2 Seed;
};

class Galaxy : public Layer
{
public:
    Galaxy(std::shared_ptr<PerspectiveCamera> camera, bool enabled);
    ~Galaxy() override;
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
    ComputePipeline *m_SimulatePipeline;

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Rotation = { -50.0f, 0.0f, 0.0f };
    glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
    friend class MainLayer;
};
