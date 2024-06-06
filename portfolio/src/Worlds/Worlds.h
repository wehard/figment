#pragma once

#include "Figment.h"
#include "Galaxy.h"
#include "ParticleRenderer.h"

using namespace Figment;

struct WorldData
{
    WebGPUTexture *ColorMap;
    WebGPUTexture *HeightMap;
    BindGroup *ComputeBindGroup;
    float RelativeSize = 1.0f;
};

// struct WorldParticle
// {
//     glm::vec3 Position;
//     uint32_t _Padding[1];
//     glm::vec4 Color;
//     glm::vec3 Velocity;
//     uint32_t _Padding2[1];
//     glm::vec3 PrevPosition;
//     uint32_t _Padding3[1];
// };

struct WorldParticlesData
{
    float DeltaTime;
    float Rotation;
    float BumpMultiplier;
    float RelativeSize;
    glm::vec2 MouseWorldPosition;
};

class Worlds : public Layer
{
public:
    Worlds(std::shared_ptr<PerspectiveCamera> camera, bool enabled);
    ~Worlds() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
    WebGPUTexture *GetTexture() { return m_WorldData[m_CurrentWorld].ColorMap; }
    WebGPUTexture *GetHeightMap() { return m_WorldData[m_CurrentWorld].HeightMap; }
    uint32_t GetParticleCount();
    float RotationSpeed = 10.0f;
    float BumpMultiplier = 0.06f;
    bool AutoCycleWorlds = true;
    float ParticleSize = 0.002f;
    void ResetParticles();
private:

    void LoadWorld(const std::string &colorMapPath, const std::string &heightMapPath, float relativeSize);
    void RecreatePipelines();

    std::shared_ptr<WebGPUContext> m_Context;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<ParticleRenderer> m_Renderer;
    std::unique_ptr<WebGPUShader> m_ComputeShader;
    std::unique_ptr<WebGPUShader> m_ParticleShader;
    std::unique_ptr<WebGPUVertexBuffer<Particle>> m_VertexBuffer;
    std::unique_ptr<WebGPUUniformBuffer<WorldParticlesData>> m_UniformBuffer;
    std::vector<WorldData> m_WorldData;
    uint32_t m_CurrentWorld = 0;
    uint32_t m_ParticleCount = 1024 * 1024;
    ComputePipeline *m_InitPipeline;
    ComputePipeline *m_SimulatePipeline;
    float m_TimeSinceLastCycle = 0.0f;
    float m_CycleInterval = 3.0f;

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
};
