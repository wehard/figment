#include "WorldMap.h"
#include "ComputePass.h"

WorldMap::WorldMap(SharedPtr<PerspectiveCamera> camera, bool enabled) : Layer("World Map", enabled), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext();

    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*m_Context);
    m_ComputeShader = CreateUniquePtr<WebGPUShader>(m_Context->GetDevice(),
            *Utils::LoadFile2("res/shaders/wgsl/world_compute.wgsl"), "WorldCompute");
    m_ParticleShader = CreateUniquePtr<WebGPUShader>(m_Context->GetDevice(),
            *Utils::LoadFile2("res/shaders/wgsl/particle.wgsl"), "Particle");
    m_VertexBuffer = CreateUniquePtr<WebGPUVertexBuffer<Particle>>
            (m_Context->GetDevice(), "ParticlesBuffer",
                    16384 * sizeof(Particle));
    auto layout = std::vector<WGPUVertexAttribute>({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
            },
    });
    m_VertexBuffer->SetVertexLayout(layout, sizeof(Particle), WGPUVertexStepMode_Vertex);
    m_UniformBuffer = CreateUniquePtr<WebGPUUniformBuffer<ParticlesData>>(m_Context->GetDevice(),
            "ParticlesDataUniformBuffer", sizeof(ParticlesData));

    auto image = Image::Load("res/2k_earth_daymap.png");
    m_WorldTexture = WebGPUTexture::Create(m_Context->GetDevice(), image);
}

WorldMap::~WorldMap()
{
    delete m_WorldTexture;
}

void WorldMap::OnAttach()
{
    ParticlesData d = {};
    d.DeltaTime = 0.0;
    d.Seed = glm::vec2(0, 0);
    m_UniformBuffer->SetData(&d, sizeof(ParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_ComputeShader.get());
    computePass.Begin();
    computePass.Bind(*m_VertexBuffer);
    computePass.Bind(*m_UniformBuffer);
    computePass.Bind(*m_WorldTexture);
    computePass.Dispatch("init", m_VertexBuffer->Count());
    computePass.End();
}

void WorldMap::OnDetach()
{

}

void WorldMap::OnUpdate(float deltaTime)
{
    m_Renderer->Begin(*m_Camera);
    m_Renderer->DrawPoints(*m_VertexBuffer, m_VertexBuffer->Count(), *m_ParticleShader);
    m_Renderer->End();
}

void WorldMap::OnImGuiRender()
{

}

void WorldMap::OnEvent(AppEvent event, void *eventData)
{

}