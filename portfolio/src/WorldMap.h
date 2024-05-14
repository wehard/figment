#pragma once
#include "Figment.h"
#include "Particles.h"
#include "ParticleRenderer.h"

using namespace Figment;

struct WorldData
{
    WebGPUTexture *ColorMap;
    WebGPUTexture *HeightMap;
    BindGroup *ComputeBindGroup;
};

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
    float BumpMultiplier;
    float _Padding[1];
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
    WebGPUTexture *GetTexture() { return m_WorldData[m_CurrentWorld].ColorMap; }
    WebGPUTexture *GetHeightMap() { return m_WorldData[m_CurrentWorld].HeightMap; }
    uint32_t GetParticleCount();
    float RotationSpeed = 10.0f;
    float BumpMultiplier = 0.06f;
    bool AutoCycleWorlds = true;
    void ResetParticles();
private:

    void LoadWorld(const std::string &colorMapPath, const std::string &heightMapPath);
    void RecreatePipelines();

    std::shared_ptr<WebGPUContext> m_Context;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<ParticleRenderer> m_Renderer;
    std::unique_ptr<WebGPUShader> m_ComputeShader;
    std::unique_ptr<WebGPUShader> m_ParticleShader;
    std::unique_ptr<WebGPUVertexBuffer<WorldParticle>> m_VertexBuffer;
    std::unique_ptr<WebGPUUniformBuffer<WorldParticlesData>> m_UniformBuffer;
    std::vector<WorldData> m_WorldData;
    uint32_t m_CurrentWorld = 0;
    uint32_t m_ParticleCount = 1024 * 1024;
    float m_Rotation = -120.0f;
    ComputePipeline *m_InitPipeline;
    ComputePipeline *m_SimulatePipeline;
    float m_TimeSinceLastCycle = 0.0f;
    float m_CycleInterval = 2.0f;
};
