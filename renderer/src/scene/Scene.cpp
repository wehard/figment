#include "Scene.h"
#include "Entity.h"

#ifndef __EMSCRIPTEN__
#include "ScriptEngine.h"
#endif

Scene::Scene() = default;

Scene::Scene(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height), m_ClearColor(glm::vec4(0.1, 0.1, 0.1, 1.0))
{
    m_Camera = std::make_shared<PerspectiveCamera>((float)width / (float)height);
    m_CameraController = std::make_shared<CameraController>(m_Camera);
    m_Camera->m_Position.z = 60.0;
}

Scene::~Scene() = default;

static glm::vec4 GetRandomColor()
{
    return { (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX,
             1.0 };
}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = { m_Registry.create(), this };
    auto &info = entity.AddComponent<InfoComponent>();
    info.m_Name = name.empty() ? "Unnamed" : name;

    auto &id = entity.AddComponent<IdComponent>();
    printf("Created entity with uuid %llu\n", (uint64_t)id.UUID);

    entity.AddComponent<TransformComponent>();
    auto color = GetRandomColor();
    color.a = 0.5f;
    entity.AddComponent<ColorComponent>(color);
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
        entities.emplace_back( e, this );
    }
    return entities;
}

Entity Scene::GetEntityById(uint32_t id)
{
    if (m_Registry.valid((entt::entity)id))
        return { (entt::entity)id, this };
    printf("Failed to find entity with id %u\n", id);
    return {};
}

Entity Scene::GetHoveredEntity()
{
    return GetEntityById(m_HoveredId);
}

void Scene::Update(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize, WebGPURenderer &renderer)
{
    m_CameraController->Update(deltaTime);

    glm::vec2 normalized = glm::vec2(mousePosition.x / viewportSize.x, mousePosition.y / viewportSize.y);
    if (m_CameraController->IsFpsCamera() || normalized.x < -1.0 || normalized.x > 1.0 || normalized.y < -1.0 || normalized.y > 1.0)
    {
        m_HoveredId = -1;
    }
    else
    {
        renderer.ReadPixel((int)mousePosition.x, (int)mousePosition.y, [this](uint32_t id)
        {
            m_HoveredId = id;
        });
    }

    auto view = m_Registry.view<TransformComponent>();
    for (auto e : view)
    {
        Entity entity = { e, this };

        if (entity.HasComponent<VerletBodyComponent>())
            m_VerletPhysics.Update(entity, GetEntities(), deltaTime);
        auto color = entity.GetHandle() == m_HoveredId ? glm::vec4(1.0, 1.0, 1.0, 1.0) : entity.GetComponent<ColorComponent>().m_Color;
        renderer.DrawQuad(entity.GetComponent<TransformComponent>().GetTransform(),
                color, entity.GetHandle());
    }
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
}
