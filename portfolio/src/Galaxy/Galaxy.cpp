#include "Galaxy.h"
#include "ComputePass.h"
#include "Figment.h"

Galaxy::Galaxy(PerspectiveCamera &camera, bool enabled) : Layer("Galaxy", enabled),
        m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext<WebGPUContext>();
    m_Renderer = std::make_unique<ParticleRenderer>(*m_Context);
    m_ComputeShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(),
            "res/shaders/particles.wgsl", "ParticlesCompute");
    m_ParticleShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(),
            "res/shaders/particle_disc_billboard.wgsl", "ParticleDiscBillboardShader");

    m_VertexBuffer = std::make_unique<WebGPUVertexBuffer<Particle>>
            (m_Context->GetDevice(), "ParticlesBuffer",
                    32768 * sizeof(Particle));

    auto layout = std::vector<WGPUVertexAttribute>({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 1,
            },
            {
                    .format = WGPUVertexFormat_Float32x4,
                    .offset = 16,
                    .shaderLocation = 2,
            },
    });
    m_VertexBuffer->SetVertexLayout(layout, sizeof(Particle), WGPUVertexStepMode_Instance);

    m_UniformBuffer = std::make_unique<WebGPUUniformBuffer<ParticlesData>>(m_Context->GetDevice(),
            "ParticlesDataUniformBuffer", sizeof(ParticlesData));

    m_ComputeBindGroup = new BindGroup(m_Context->GetDevice(), WGPUShaderStage_Compute);
    m_ComputeBindGroup->Bind(*m_VertexBuffer);
    m_ComputeBindGroup->Bind(*m_UniformBuffer);

    // m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_ComputeBindGroup);
    // m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());

    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
}

Galaxy::~Galaxy()
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

void Galaxy::OnAttach()
{

}

void Galaxy::OnDetach()
{

}

void Galaxy::OnEnable()
{
    m_Camera.SetPosition(glm::vec3(0.0, 0.0, 2.0));
}

void Galaxy::OnDisable()
{

}

void Galaxy::OnUpdate(float deltaTime)
{
    ParticlesData d = {};
    d.DeltaTime = deltaTime;
    d.Seed = glm::vec2(0);
    d.Time = App::Instance()->GetTimeSinceStart();
    m_UniformBuffer->SetData(&d, sizeof(ParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_SimulatePipeline, m_ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("simulate", m_VertexBuffer->Count());
    computePass.End();

    auto time = glfwGetTime();
    m_Rotation.y = cos(time * 0.25f) * 15.0f;
    m_Rotation.z = sin(time * 0.25f) * 15.0f;

    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), m_Scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), m_Position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(m_Rotation.x), glm::radians(m_Rotation.y),
            glm::radians(m_Rotation.z));
    glm::mat4 transform = matTranslate * matRotate * matScale;

    m_Renderer->BeginFrame(m_Camera);
    m_Renderer->DrawQuads(*m_VertexBuffer, transform, m_ParticleSize, *m_ParticleShader);
    m_Renderer->EndFrame();

}

void Galaxy::OnImGuiRender()
{
}

void Galaxy::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::Window::ResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}
