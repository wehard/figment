#include "Scene.h"

#include "Entity.h"

Scene::Scene()
{
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
