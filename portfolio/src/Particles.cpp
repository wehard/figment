#include "Particles.h"
#include "ComputePass.h"
#include "Figment.h"

Particles::Particles(SharedPtr<PerspectiveCamera> camera) : Layer("Particles"), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext();
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());
    m_ComputeShader = CreateUniquePtr<WebGPUShader>(m_Context->GetDevice(),
            *Utils::LoadFile2("res/shaders/wgsl/particles.wgsl"), "ParticlesCompute");
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

    m_UniformBuffer = CreateUniquePtr<WebGPUUniformBuffer<ParticlesData>>(m_Context->GetDevice(),
            "ParticlesDataUniformBuffer", sizeof(ParticlesData));
}

Particles::~Particles()
{

}

void Particles::OnAttach()
{
    FIG_LOG_INFO("Particles layer attached");

    ComputePass computePass(m_Context->GetDevice(), m_ComputeShader.get());
    computePass.Begin();
    computePass.Bind(*m_VertexBuffer);
    computePass.Bind(*m_UniformBuffer);
    computePass.Dispatch("init", 16384);
    computePass.End();

    // wgpuDevicePushErrorScope(m_Context->GetDevice(), WGPUErrorFilter_OutOfMemory);
    // wgpuDevicePushErrorScope(m_Context->GetDevice(), WGPUErrorFilter_Validation);
}

void Particles::OnDetach()
{

}

static void Error(WGPUErrorType type, char const *message, void *userdata)
{
    const char *errorType = "Unknown";
    switch (type)
    {
    case WGPUErrorType_NoError:
        errorType = "NoError";
        break;
    case WGPUErrorType_Validation:
        errorType = "Validation";
        break;
    case WGPUErrorType_OutOfMemory:
        errorType = "OutOfMemory";
        break;
    case WGPUErrorType_Internal:
        errorType = "Internal";
        break;
    case WGPUErrorType_Unknown:
        errorType = "Unknown";
        break;
    case WGPUErrorType_DeviceLost:
        errorType = "DeviceLost";
        break;
    case WGPUErrorType_Force32:
        errorType = "Force32";
        break;
    }
    FIG_LOG_ERROR("%s - %s", errorType, message);
}

void Particles::OnUpdate(float deltaTime)
{
    ParticlesData d = {};
    d.DeltaTime = deltaTime;
    m_UniformBuffer->SetData(&d, sizeof(ParticlesData));

    auto computePass = new ComputePass(m_Context->GetDevice(), m_ComputeShader.get());
    computePass->Begin();
    computePass->Bind(*m_VertexBuffer);
    computePass->Bind(*m_UniformBuffer);
    computePass->Dispatch("simulate", 16384);
    computePass->End();

    wgpuDevicePopErrorScope(m_Context->GetDevice(), &Error, nullptr);

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
