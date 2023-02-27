#include "Scene.h"

#include "Entity.h"

Scene::Scene() {}

Scene::Scene(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
{
    m_Renderer = new GLRenderer(width, height);
    m_Camera = OrthoCamera(width, height);
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
    m_Registry.destroy(entity);
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

void Scene::Update(float deltaTime, glm::vec2 mousePosition)
{
    m_Camera.OnUpdate(mousePosition);
    m_Renderer->Begin(m_Camera, m_ClearColor);

    // m_Framebuffer->Bind();
    // m_Framebuffer->ClearAttachment(0, 0);
    // m_Framebuffer->ClearAttachment(1, 0);

    // m_Renderer.DrawLines(*grid, *shader);

    auto view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity = {e, this};
        m_Renderer->DrawQuad(entity.GetComponent<TransformComponent>().GetTransform(), 42);
    }

    // m_HoveredId = m_Renderer->m_Framebuffer->GetPixel(1, (uint32_t)mousePosition.x, m_Height - (uint32_t)mousePosition.y);
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
