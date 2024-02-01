#include "Particles.h"
#include "ComputePass.h"
#include "Figment.h"

Particles::Particles(SharedPtr<PerspectiveCamera> camera, bool enabled) : Layer("Particles", enabled), m_Camera(camera)
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
                    32768 * sizeof(Particle));

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

static void Error(WGPUErrorType type, char const *message, void *userdata)
{
    const char *errorType = "Unknown";
    switch (type)
    {
    case WGPUErrorType_NoError:
        errorType = "NoError";
        return;
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

void Particles::OnAttach()
{
    // wgpuDevicePushErrorScope(m_Context->GetDevice(), WGPUErrorFilter_Internal);
    // wgpuDevicePushErrorScope(m_Context->GetDevice(), WGPUErrorFilter_Validation);
    // wgpuDevicePushErrorScope(m_Context->GetDevice(), WGPUErrorFilter_OutOfMemory);

    FIG_LOG_INFO("Particles layer attached");

    ParticlesData d = {};
    d.DeltaTime = 0.0;
    d.Seed = glm::vec2(1234, 5432);
    m_UniformBuffer->SetData(&d, sizeof(ParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_ComputeShader.get());
    computePass.Begin();
    computePass.Bind(*m_VertexBuffer);
    computePass.Bind(*m_UniformBuffer);
    computePass.Dispatch("init", m_VertexBuffer->Count());
    computePass.End();

    // wgpuDevicePopErrorScope(m_Context->GetDevice(), &Error, nullptr);
}

void Particles::OnDetach()
{

}

void Particles::OnUpdate(float deltaTime)
{
    ParticlesData d = {};
    d.DeltaTime = deltaTime;
    d.Seed = glm::vec2(0);
    m_UniformBuffer->SetData(&d, sizeof(ParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_ComputeShader.get());
    computePass.Begin();
    computePass.Bind(*m_VertexBuffer);
    computePass.Bind(*m_UniformBuffer);
    computePass.Dispatch("simulate", m_VertexBuffer->Count());
    computePass.End();

    m_Renderer->Begin(*m_Camera);
    m_Renderer->DrawPoints(*m_VertexBuffer, m_VertexBuffer->Count(), *m_ParticleShader);
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
