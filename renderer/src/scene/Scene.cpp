#include "Scene.h"
#include "Entity.h"
#include "WebGPUWindow.h"
#include "App.h"

#ifndef __EMSCRIPTEN__
#include "ScriptEngine.h"
#endif

Scene::Scene(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
{
    m_Camera = std::make_shared<PerspectiveCamera>((float)width / (float)height);
    m_CameraController = std::make_shared<CameraController>(m_Camera);
    m_Camera->GetPositionPtr()->z = 60.0;

    auto m_Window = App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<WebGPUWindow>(m_Window);
    m_Renderer = std::make_unique<WebGPURenderer>(*webGpuWindow->GetContext());
}

Scene::~Scene()
{

}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = { m_Registry.create(), this };
    auto &info = entity.AddComponent<InfoComponent>();
    info.m_Name = name.empty() ? "Unnamed" : name;

    entity.AddComponent<IdComponent>();
    entity.AddComponent<TransformComponent>();

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    if (m_Registry.valid(entity))
    {
        m_Registry.destroy(entity);
    }
}

std::vector<Entity> Scene::GetEntities()
{
    auto view = m_Registry.view<InfoComponent>();
    std::vector<Entity> entities;
    for (auto e : view)
    {
        entities.emplace_back(e, this);
    }
    return entities;
}

Entity Scene::GetEntityById(uint32_t id)
{
    if (id < 0)
        return {};
    if (m_Registry.valid((entt::entity)id))
        return { (entt::entity)id, this };
    printf("Failed to find entity with id %u\n", id);
    return {};
}

Entity Scene::GetHoveredEntity()
{
    if (m_HoveredId < 0)
        return {};
    return GetEntityById(m_HoveredId);
}

void Scene::OnUpdate(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize)
{
    m_Renderer->BeginComputePass();
    auto figments = m_Registry.view<FigmentComponent>();
    for (auto e : figments)
    {
        Entity entity = { e, this };
        if (entity.HasComponent<FigmentComponent>())
        {
            auto &figment = entity.GetComponent<FigmentComponent>();
            m_Renderer->Compute(figment.ComputeShader, *figment.Buffer, *figment.MapBuffer);
            figment.MapBuffer->MapReadAsync([&figment](const glm::vec4 *data, size_t size)
            {
                figment.Data = (glm::vec4*)data;
            });
        }
    }
    m_Renderer->EndComputePass();

    m_Renderer->Begin(*m_CameraController->GetCamera()); // TODO: Make static
    m_CameraController->Update(deltaTime);

    glm::vec2 normalized = glm::vec2(mousePosition.x / viewportSize.x, mousePosition.y / viewportSize.y);
    if (m_CameraController->IsFpsCamera() || normalized.x < -1.0 || normalized.x > 1.0 || normalized.y < -1.0
            || normalized.y > 1.0)
    {
        m_HoveredId = -1;
    }
    else
    {
        m_Renderer->ReadPixel((int)mousePosition.x, (int)mousePosition.y, [this](int32_t id)
        {
            m_HoveredId = id;
        });
    }

    auto entities = m_Registry.view<TransformComponent>();
    for (auto e : entities)
    {
        Entity entity = { e, this };
        auto &transform = entity.GetComponent<TransformComponent>();
        if (entity.HasComponent<CircleComponent>())
        {
            auto &circle = entity.GetComponent<CircleComponent>();
            auto color = entity.GetHandle() == m_HoveredId ? glm::vec4(1.0, 1.0, 1.0, 1.0) : circle.Color;
            m_Renderer->DrawCircle(transform.Position, transform.Scale, color, (int)entity.GetHandle());
        }
        if (entity.HasComponent<QuadComponent>())
        {
            auto &quad = entity.GetComponent<QuadComponent>();
            auto color = entity.GetHandle() == m_HoveredId ? glm::vec4(1.0, 1.0, 1.0, 1.0) : quad.Color;
            m_Renderer->DrawQuad(transform.Position, transform.Scale, color, (int)entity.GetHandle());
        }
    }

    auto view = m_Registry.view<FigmentComponent>();
    for (auto &e : view)
    {
        Entity entity = { e, this };
        auto &figment = entity.GetComponent<FigmentComponent>();
        auto &transform = entity.GetComponent<TransformComponent>();
        // auto &quad = entity.GetComponent<QuadComponent>();
        if (figment.Data == nullptr)
            continue;
        for (int i = 0; i < figment.Buffer->GetSize() / sizeof(glm::vec4); i++)
        {
            m_Renderer->DrawCircle(transform.Position + glm::vec3(figment.Data[i].x, figment.Data[i].y, figment.Data[i].z), transform.Scale,
                    figment.Color, (int)entity.GetHandle());
        }
    }

    m_Renderer->End();
}

std::shared_ptr<CameraController> Scene::GetCameraController()
{
    return m_CameraController;
}

void Scene::OnResize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    m_Camera->Resize((float)width, (float)height);
    m_Renderer->OnResize(width, height);
}

