#include "Worlds.h"

Worlds::Worlds(PerspectiveCamera &camera, bool enabled) : Layer("Worlds", enabled), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext<WebGPUContext>();

    m_Renderer = std::make_unique<ParticleRenderer>(*m_Context);
    m_ComputeShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(), "res/shaders/world_compute.wgsl",
            "WorldCompute");
    m_ParticleShader = std::make_unique<WebGPUShader>(m_Context->GetDevice(),
            "res/shaders/particle_disc_billboard_worlds.wgsl",
            "WorldParticle");
    m_VertexBuffer = std::make_unique<WebGPUVertexBuffer<Particle >>
            (m_Context->GetDevice(), "ParticlesBuffer",
                    m_ParticleCount * sizeof(Particle));
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
            }
    });
    m_VertexBuffer->SetVertexLayout(layout, sizeof(Particle), WGPUVertexStepMode_Instance);
    m_UniformBuffer = std::make_unique<WebGPUUniformBuffer<WorldParticlesData >>(m_Context->GetDevice(),
            "ParticlesDataUniformBuffer", sizeof(WorldParticlesData));

    LoadWorld("res/earth_color_map.png", "res/earth_height_map.jpg", 1.0);
    LoadWorld("res/moon_color_map.png", "res/moon_height_map.jpg", 0.27);
    LoadWorld("res/mars_color_map.jpg", "res/mars_height_map.jpg", 0.53);

    m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());

    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());

    m_OverlayRenderer = std::make_unique<OverlayRenderer>(*m_Context);
}

Worlds::~Worlds()
{
    for (auto &wd : m_WorldData)
    {
        delete wd.ColorMap;
        delete wd.HeightMap;
        delete wd.ComputeBindGroup;
    }
}

void Worlds::LoadWorld(const std::string &colorMapPath, const std::string &heightMapPath, float relativeSize)
{
    auto colorMapImage = Image::Load(colorMapPath);
    auto colorMap = WebGPUTexture::Create(m_Context->GetDevice(), colorMapImage);
    auto heightMapImage = Image::Load(heightMapPath);
    auto heightMap = WebGPUTexture::Create(m_Context->GetDevice(), heightMapImage);

    auto m_ComputeBindGroup = new BindGroup(m_Context->GetDevice(), WGPUShaderStage_Compute);
    m_ComputeBindGroup->Bind(*m_VertexBuffer);
    m_ComputeBindGroup->Bind(*m_UniformBuffer);
    m_ComputeBindGroup->Bind(*colorMap);
    m_ComputeBindGroup->Bind(*heightMap);

    m_WorldData.push_back({
            .ColorMap = colorMap,
            .HeightMap = heightMap,
            .ComputeBindGroup = m_ComputeBindGroup,
            .RelativeSize = relativeSize,
    });
}

void Worlds::OnAttach()
{
    ResetParticles();
}

void Worlds::OnDetach()
{

}

static bool intersectSphere(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 sphereOrigin, float sphereRadius,
        glm::vec3 *hitPoint)
{
    glm::vec3 oc = rayOrigin - sphereOrigin;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0f * dot(oc, rayDirection);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return false;
    }
    else
    {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        *hitPoint = rayOrigin + rayDirection * t;
        return true;
    }
}

void Worlds::OnUpdate(float deltaTime)
{
    m_TimeSinceLastCycle += deltaTime;
    if (Cycle && m_TimeSinceLastCycle >= m_CycleInterval)
    {
        m_CurrentWorld = (m_CurrentWorld + 1) % m_WorldData.size();
        m_TimeSinceLastCycle = 0.0;
        RecreatePipelines();
    }

    if (Rotate)
        Rotation.y += RotationSpeed * deltaTime;

    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), Scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), Position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y),
            glm::radians(Rotation.z));
    glm::mat4 transform = matTranslate * matRotate * matScale;

    auto invModel = glm::inverse(transform);

    auto mouseWorld = m_Camera.ScreenToWorldSpace(Input::GetMousePosition(),
            glm::vec2(m_Context->GetSwapChainWidth(), m_Context->GetSwapChainHeight()));

    WorldParticlesData worldData = {};
    worldData.DeltaTime = deltaTime;
    worldData.BumpMultiplier = BumpMultiplier;
    worldData.MouseWorldPosition = { 0, 0, 0 };
    worldData.RelativeSize = m_WorldData[m_CurrentWorld].RelativeSize;
    worldData.Time = (float)glfwGetTime();

    auto rayDir = glm::normalize(mouseWorld - m_Camera.GetPosition());

    auto hitPoint = glm::vec3(0);
    if (intersectSphere(m_Camera.GetPosition(), rayDir, Position,
            m_WorldData[m_CurrentWorld].RelativeSize, &hitPoint))
    {
        worldData.MouseWorldPosition = glm::vec3(invModel * glm::vec4(hitPoint, 1.0));;
    }
    m_HitPoint = hitPoint;

    m_UniformBuffer->SetData(&worldData, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_SimulatePipeline, m_WorldData[m_CurrentWorld].ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("simulate", m_VertexBuffer->Count() / 32);
    computePass.End();

    m_Renderer->BeginFrame(m_Camera);
    m_Renderer->DrawQuads(*m_VertexBuffer, transform, ParticleSize, *m_ParticleShader);
    m_Renderer->EndFrame();

    m_OverlayRenderer->Begin(m_Camera);
    m_OverlayRenderer->DrawGrid();
    m_OverlayRenderer->End();
}

void Worlds::OnImGuiRender()
{
    bool shouldRecreatePipeline = false;
    auto size = ImGui::GetWindowWidth();
    ImGui::SetNextWindowPos(ImVec2(m_Context->GetSwapChainWidth() / 2, m_Context->GetSwapChainHeight() - 80),
            ImGuiCond_Always, ImVec2(0.5, 1.0));
    ImGui::Begin("World", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
    ImVec2 buttonSize = ImVec2(30, 20);
    auto buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    buttonColor.x *= 0.5;
    buttonColor.y *= 0.5;
    buttonColor.z *= 0.5;
    buttonColor.w = 1.0;
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    if (ImGui::Button("<", buttonSize))
    {
        if (m_CurrentWorld == 0)
        {
            m_CurrentWorld = m_WorldData.size() - 1;
        }
        else
        {
            m_CurrentWorld = (m_CurrentWorld - 1) % m_WorldData.size();
        }
        shouldRecreatePipeline = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(">", buttonSize))
    {
        m_CurrentWorld = (m_CurrentWorld + 1) % m_WorldData.size();
        shouldRecreatePipeline = true;
    }
    ImGui::PopStyleColor();
    ImGui::End();

    if (shouldRecreatePipeline)
    {
        m_TimeSinceLastCycle = 0.0;
        RecreatePipelines();
    }
}

void Worlds::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}

uint32_t Worlds::GetParticleCount()
{
    return m_ParticleCount;
}

void Worlds::ResetParticles()
{
    WorldParticlesData d = {};
    d.DeltaTime = 0.0;
    d.BumpMultiplier = BumpMultiplier;
    d.MouseWorldPosition = { 0, 0, 0 };
    d.RelativeSize = m_WorldData[m_CurrentWorld].RelativeSize;
    m_UniformBuffer->SetData(&d, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_InitPipeline, m_WorldData[m_CurrentWorld].ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("init", m_VertexBuffer->Count() / 32);
    computePass.End();
}

void Worlds::RecreatePipelines()
{
    delete m_InitPipeline;
    delete m_SimulatePipeline;
    m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());
    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
}
