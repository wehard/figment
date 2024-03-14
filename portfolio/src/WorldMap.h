#pragma once
#include "Figment.h"
#include "Particles.h"
#include "ParticleRenderer.h"

using namespace Figment;

struct WorldParticle
{
    glm::vec3 Position;
    uint32_t _Padding[1];
    glm::vec4 Color;
    glm::vec3 Velocity;
    uint32_t _Padding2[1];
    glm::vec3 PrevPosition;
    uint32_t _Padding3[1];
};

struct WorldParticlesData
{
    float DeltaTime;
    float Rotation;
    glm::vec2 MouseWorldPosition;
};

class WorldMap : public Layer
{
public:
    WorldMap(std::shared_ptr<PerspectiveCamera> camera, bool enabled);
    ~WorldMap() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
    WebGPUTexture *GetTexture();
    WebGPUTexture *GetHeightMap()
    { return m_WorldHeightMap; }
    uint32_t GetParticleCount();
    float RotationSpeed = 10.0f;
    void ResetParticles();
private:
    std::shared_ptr<WebGPUContext> m_Context;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<ParticleRenderer> m_Renderer;
    std::unique_ptr<WebGPUShader> m_ComputeShader;
    std::unique_ptr<WebGPUShader> m_ParticleShader;
    std::unique_ptr<WebGPUVertexBuffer<WorldParticle>> m_VertexBuffer;
    std::unique_ptr<WebGPUUniformBuffer<WorldParticlesData>> m_UniformBuffer;
    WebGPUTexture *m_WorldTexture;
    WebGPUTexture *m_WorldHeightMap;
    uint32_t m_ParticleCount = 1024 * 1024;
    float m_Rotation = -120.0f;
    ComputePipeline *m_InitPipeline;
    ComputePipeline *m_SimulatePipeline;
    BindGroup *m_ComputeBindGroup;
};
