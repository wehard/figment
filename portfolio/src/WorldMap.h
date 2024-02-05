#pragma once
#include "Figment.h"
#include "Particles.h"

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
    WorldMap(SharedPtr<PerspectiveCamera> camera, bool enabled);
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
    SharedPtr<WebGPUContext> m_Context;
    SharedPtr<PerspectiveCamera> m_Camera;
    UniquePtr<WebGPURenderer> m_Renderer;
    UniquePtr<WebGPUShader> m_ComputeShader;
    UniquePtr<WebGPUShader> m_ParticleShader;
    UniquePtr<WebGPUVertexBuffer<WorldParticle>> m_VertexBuffer;
    UniquePtr<WebGPUUniformBuffer<WorldParticlesData>> m_UniformBuffer;
    WebGPUTexture *m_WorldTexture;
    WebGPUTexture *m_WorldHeightMap;
    uint32_t m_ParticleCount = 1024 * 1024;
    float m_Rotation = -120.0f;
};
