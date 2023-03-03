#include "Scene.h"

#include "Entity.h"

Scene::Scene() {}

Scene::Scene(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
{
    m_Renderer = new GLRenderer(width, height);
    m_Camera = OrthoCamera(width, height);
    m_ClearColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    entity.AddComponent<IDComponent>();
    auto &info = entity.AddComponent<InfoComponent>();
    info.m_Name = name.empty() ? "Unnamed" : name;
    entity.AddComponent<TransformComponent>();
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

Entity Scene::GetHoveredEntity()
{
    if (m_Registry.valid((entt::entity)m_HoveredId))
        return {(entt::entity)m_HoveredId, this};
    return {(entt::entity)0, this};
}

void Scene::Update(float deltaTime, glm::vec2 mousePosition)
{
    m_Camera.OnUpdate(mousePosition);
    m_Renderer->Begin(m_Camera, m_ClearColor);

    auto view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity = {e, this};
        m_Renderer->DrawCircle(entity.GetComponent<TransformComponent>().GetTransform(), glm::vec4(1.0), (int)e);
    }

    m_HoveredId = m_Renderer->m_Framebuffer->GetPixel(1, (uint32_t)mousePosition.x, m_Height - (uint32_t)mousePosition.y);
    m_Renderer->End();
}

OrthoCamera &Scene::GetCamera()
{
    return m_Camera;
}

void Scene::OnResize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    m_Camera.OnResize(width, height);
    m_Renderer->OnResize(width, height);
}
