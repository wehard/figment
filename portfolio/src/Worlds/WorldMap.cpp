#include "WorldMap.h"

WorldMap::WorldMap(std::shared_ptr<PerspectiveCamera> camera, bool enabled) : Layer("World", enabled), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Context = webGpuWindow->GetContext<WebGPUContext>();

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

    LoadWorld("res/earth_color_map.png", "res/earth_height_map.jpg", 1.0);
    LoadWorld("res/moon_color_map.png", "res/moon_height_map.jpg", 0.27);
    LoadWorld("res/mars_color_map.jpg", "res/mars_height_map.jpg", 0.53);

    m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());

    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
}

WorldMap::~WorldMap()
{
    for (auto &wd : m_WorldData)
    {
        delete wd.ColorMap;
        delete wd.HeightMap;
        delete wd.ComputeBindGroup;
    }
}

void WorldMap::LoadWorld(const std::string &colorMapPath, const std::string &heightMapPath, float relativeSize)
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
    m_TimeSinceLastCycle += deltaTime;
    if (AutoCycleWorlds && m_TimeSinceLastCycle >= m_CycleInterval)
    {
        m_CurrentWorld = (m_CurrentWorld + 1) % m_WorldData.size();
        m_TimeSinceLastCycle = 0.0;
        RecreatePipelines();
    }
    m_Rotation -= RotationSpeed * deltaTime;

    auto mw = m_Camera->ScreenToWorldSpace(Input::GetMousePosition(),
            glm::vec2(m_Context->GetSwapChainWidth(), m_Context->GetSwapChainHeight()));

    WorldParticlesData d = {};
    d.DeltaTime = deltaTime;
    d.Rotation = m_Rotation;
    d.BumpMultiplier = BumpMultiplier;
    d.MouseWorldPosition = mw;
    d.RelativeSize = m_WorldData[m_CurrentWorld].RelativeSize;

    auto rd = mw - m_Camera->GetPosition();
    rd = glm::normalize(rd);
    float t = 0.0;
    if (intersectSphere(m_Camera->GetPosition(), rd, glm::vec3(0, 0, 0), 1.0, &t))
    {
        auto p = m_Camera->GetPosition() + rd * t;
        d.MouseWorldPosition = p;
    }

    m_UniformBuffer->SetData(&d, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_SimulatePipeline, m_WorldData[m_CurrentWorld].ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("simulate", m_VertexBuffer->Count() / 32);
    computePass.End();

    m_Renderer->BeginFrame(*m_Camera);
    m_Renderer->DrawPoints(*m_VertexBuffer, *m_ParticleShader);
    m_Renderer->EndFrame();
}

void WorldMap::OnImGuiRender()
{
    bool shouldRecreatePipeline = false;
    auto size = ImGui::GetWindowWidth();
    ImGui::SetNextWindowPos(ImVec2(m_Context->GetSwapChainWidth() / 2, m_Context->GetSwapChainHeight() - 80),
            ImGuiCond_Always, ImVec2(0.5, 1.0));
    ImGui::Begin("World", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
    ImVec2 buttonSize = ImVec2(40, 40);
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

void WorldMap::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
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
    d.RelativeSize = m_WorldData[m_CurrentWorld].RelativeSize;
    m_UniformBuffer->SetData(&d, sizeof(WorldParticlesData));

    ComputePass computePass(m_Context->GetDevice(), m_InitPipeline, m_WorldData[m_CurrentWorld].ComputeBindGroup);
    computePass.Begin();
    computePass.Dispatch("init", m_VertexBuffer->Count() / 32);
    computePass.End();
}

void WorldMap::RecreatePipelines()
{
    delete m_InitPipeline;
    delete m_SimulatePipeline;
    m_InitPipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_InitPipeline->Build("init", m_ComputeShader->GetShaderModule());
    m_SimulatePipeline = new ComputePipeline(m_Context->GetDevice(), *m_WorldData[m_CurrentWorld].ComputeBindGroup);
    m_SimulatePipeline->Build("simulate", m_ComputeShader->GetShaderModule());
}