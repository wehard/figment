#include "Scene.h"
#include "Entity.h"

#ifndef __EMSCRIPTEN__
#include "ScriptEngine.h"
#endif

Scene::Scene() = default;

Scene::Scene(uint32_t width, uint32_t height) : m_Width(width), m_Height(height), m_ClearColor(glm::vec4(0.1, 0.1, 0.1, 1.0))
{
    m_Renderer = new GLRenderer(width, height);
    m_Camera = std::make_shared<PerspectiveCamera>((float)width / (float)height);
    m_CameraController = std::make_shared<CameraController>(m_Camera);
//    m_CameraController->GetCamera()->SetPosition(glm::vec3(0.0, 0.0, 60.0));
}

Scene::~Scene() = default;

static glm::vec4 GetRandomColor()
{
    return {(float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0};
}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    entity.AddComponent<IDComponent>();
    auto &info = entity.AddComponent<InfoComponent>();
    info.m_Name = name.empty() ? "Unnamed" : name;
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<ColorComponent>(GetRandomColor());
    return entity;
}

Entity Scene::CreateEntity(uint32_t id)
{
    return {m_Registry.create((entt::entity)id), this};
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
        entities.push_back({e, this});
    }
    return entities;
}

Entity Scene::GetEntityById(uint32_t id)
{
    if (m_Registry.valid((entt::entity)id))
        return {(entt::entity)id, this};
    return {};
}

Entity Scene::GetHoveredEntity()
{
    if (m_Registry.valid((entt::entity)m_HoveredId))
        return {(entt::entity)m_HoveredId, this};
    return {};
}

void Scene::Update(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize)
{
    m_CameraController->Update(deltaTime);
    m_Renderer->Begin(m_Camera, m_ClearColor);

    auto t = TransformComponent();
    t.Scale = glm::vec3(m_VerletPhysics.GetWidth() + 1, m_VerletPhysics.GetHeight() + 1, 1);
    t.Position = glm::vec3(0);
    m_Renderer->DrawQuad(t.GetTransform(), glm::vec4(0.1, 0.3, 0.8, 0.2), -1);

    auto view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity = {e, this};


        if (entity.HasComponent<VerletBodyComponent>())
            m_VerletPhysics.Update(entity, GetEntities(), deltaTime);
        m_Renderer->DrawCircle(entity.GetComponent<TransformComponent>().GetTransform(), entity.GetComponent<ColorComponent>().m_Color, (int)e, m_HoveredId);
    }

#ifndef __EMSCRIPTEN__
    // ScriptEngine::s_Instance->OnUpdate(deltaTime);
#endif

    glm::vec2 normalized = glm::vec2(mousePosition.x / viewportSize.x, mousePosition.y / viewportSize.y);
    if (normalized.x < -1.0 || normalized.x > 1.0 || normalized.y < -1.0 || normalized.y > 1.0)
    {
        m_HoveredId = -1;
    }
    else
    {
        m_HoveredId = m_Renderer->GetFramebuffer()->GetPixel(1, (uint32_t)(normalized.x * m_Width), m_Height - (uint32_t)(normalized.y * m_Height));
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
