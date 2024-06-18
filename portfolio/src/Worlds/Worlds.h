#pragma once

#include "Figment.h"
#include "Galaxy.h"
#include "ParticleRenderer.h"
#include "OverlayRenderer.h"

using namespace Figment;

struct WorldData
{
    WebGPUTexture *ColorMap = nullptr;
    WebGPUTexture *HeightMap = nullptr;
    BindGroup *ComputeBindGroup = nullptr;
    float RelativeSize = 1.0f;
};

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
    void ResetParticles();

public:
    glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
    bool Rotate = true;
    float RotationSpeed = 10.0f;
    float BumpMultiplier = 0.06f;
    bool Cycle = true;
    float ParticleSize = 0.002f;

private:
    void LoadWorld(const std::string &colorMapPath, const std::string &heightMapPath, float relativeSize);
    void RecreatePipelines();

private:
    std::shared_ptr<WebGPUContext> m_Context;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<ParticleRenderer> m_Renderer;
    std::unique_ptr<OverlayRenderer> m_OverlayRenderer;
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
};
