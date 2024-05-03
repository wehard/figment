#include "WorldMap.h"

WorldMap::WorldMap(std::shared_ptr<PerspectiveCamera> camera, bool enabled) : Layer("World", enabled), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext();

    m_Renderer = std::make_unique<ParticleRenderer>(*m_Context);
    m_ComputeShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(), "res/shaders/world_compute.wgsl",
            "WorldCompute");
    m_ParticleShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(), "res/shaders/world_particle.wgsl",
            "WorldParticle");
    m_VertexBuffer = std::make_unique<WebGPUVertexBuffer<WorldParticle>>
            (m_Context->GetDevice(), "ParticlesBuffer",
                    m_ParticleCount * sizeof(WorldParticle));
    auto layout = std::vector<WGPUVertexAttribute>({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
            },
            {
                    .format = WGPUVertexFormat_Float32x4,
                    .offset = 16,
                    .shaderLocation = 1,
            },
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 32,
                    .shaderLocation = 2,
            },
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 48,
                    .shaderLocation = 3,
            },
    });
    m_VertexBuffer->SetVertexLayout(layout, sizeof(WorldParticle), WGPUVertexStepMode_Vertex);
    m_UniformBuffer = std::make_unique<WebGPUUniformBuffer<WorldParticlesData>>(m_Context->GetDevice(),
            "ParticlesDataUniformBuffer", sizeof(WorldParticlesData));

    auto map = Image::Load("res/2k_earth_daymap.png");
    m_WorldTexture = WebGPUTexture::Create(m_Context->GetDevice(), map);
    auto bump = Image::Load("res/2k_earth_bump.jpg");
    m_WorldHeightMap = WebGPUTexture::Create(m_Context->GetDevice(), bump);

    m_ComputeBindGroup = new BindGroup(m_Context->GetDevice(), WGPUShaderStage_Compute);
    m_ComputeBindGroup->Bind(*m_VertexBuffer);
    m_ComputeBindGroup->Bind(*m_UniformBuffer);
    m_ComputeBindGroup->Bind(*m_WorldTexture);
    m_ComputeBindGroup->Bind(*m_WorldHeightMap);

    m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_ComputeBindGroup);
    m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());

    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
}

WorldMap::~WorldMap()
{
    delete m_WorldTexture;
}

void WorldMap::OnAttach()
{
    ResetParticles();
}

void WorldMap::OnDetach()
{

}

static bool intersectSphere(glm::vec3 ro, glm::vec3 rd, glm::vec3 sc, float sr, float *t)
{
    glm::vec3 oc = ro - sc;
    float b = glm::dot(oc, rd);
    float c = glm::dot(oc, oc) - sr * sr;
    float h = b * b - c;
    if (h < 0.0)
    {
        return false;
    }
    h = sqrt(h);
    *t = -b - h;
    if (*t < 0.0)
    {
        *t = -b + h;
    }
    return *t >= 0.0;
}

void WorldMap::OnUpdate(float deltaTime)
{
    m_Rotation -= RotationSpeed * deltaTime;

    auto mw = m_Camera->ScreenToWorldSpace(Input::GetMousePosition(),
            glm::vec2(m_Context->GetSwapChainWidth(), m_Context->GetSwapChainHeight()));

    WorldParticlesData d = {};
    d.DeltaTime = deltaTime;
    d.Rotation = m_Rotation;
    d.BumpMultiplier = BumpMultiplier;
    d.MouseWorldPosition = mw;

    auto rd = mw - m_Camera->GetPosition();
    rd = glm::normalize(rd);
    float t = 0.0;
    if (intersectSphere(m_Camera->GetPosition(), rd, glm::vec3(0, 0, 0), 1.0, &t))
    {
        auto p = m_Camera->GetPosition() + rd * t;
        d.MouseWorldPosition = p;
    }

    m_UniformBuffer->SetData(&d, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_SimulatePipeline, m_ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("simulate", m_VertexBuffer->Count() / 32);
    computePass.End();

    m_Renderer->BeginFrame(*m_Camera);
    m_Renderer->DrawPoints(*m_VertexBuffer, *m_ParticleShader);
    m_Renderer->EndFrame();
}

void WorldMap::OnImGuiRender()
{
    // ImGui::SetNextWindowPos(ImVec2(10, 500), ImGuiCond_Once);
    // ImGui::Begin("Texture");
    // ImGui::Image((void *)m_WorldTexture->GetTextureView(),
    //         ImVec2((float)m_WorldTexture->GetWidth() / 4, (float)m_WorldTexture->GetHeight() / 4));
    // ImGui::End();
}

void WorldMap::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}

WebGPUTexture *WorldMap::GetTexture()
{
    return m_WorldTexture;
}

uint32_t WorldMap::GetParticleCount()
{
    return m_ParticleCount;
}

void WorldMap::ResetParticles()
{
    WorldParticlesData d = {};
    d.DeltaTime = 0.0;
    d.Rotation = m_Rotation;
    d.BumpMultiplier = BumpMultiplier;
    d.MouseWorldPosition = glm::vec2(0, 0);
    m_UniformBuffer->SetData(&d, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_InitPipeline, m_ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("init", m_VertexBuffer->Count() / 32);
    computePass.End();
}
