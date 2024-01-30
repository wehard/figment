#include "Particles.h"
#include "ComputePass.h"

Particles::Particles(SharedPtr<PerspectiveCamera> camera) : Layer("Particles"), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext();
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());
    m_Shader = CreateUniquePtr<WebGPUShader>(m_Context->GetDevice(),
            *Utils::LoadFile2("res/shaders/wgsl/particles.wgsl"));
    m_ParticleShader = CreateUniquePtr<WebGPUShader>(m_Context->GetDevice(),
            *Utils::LoadFile2("res/shaders/wgsl/particle.wgsl"));

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
}

Particles::~Particles()
{

}

void Particles::OnAttach()
{
    FIG_LOG_INFO("Particles layer attached");
    ComputePass computePass(m_Context->GetDevice(), *m_Shader);
    computePass.Begin();
    computePass.Bind(m_VertexBuffer->GetBuffer(), m_VertexBuffer->GetSize());
    computePass.Dispatch("init", 1024);
    computePass.End();
}

void Particles::OnDetach()
{

}

void Particles::OnUpdate(float deltaTime)
{
    m_Renderer->Begin(*m_Camera);
    m_Renderer->DrawPoints(*m_VertexBuffer, 16384, *m_ParticleShader);
    m_Renderer->End();
}

void Particles::OnImGuiRender()
{

}

void Particles::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}
