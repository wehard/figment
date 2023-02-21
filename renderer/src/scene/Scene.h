#pragma once

#include "entt.hpp"
#include <vector>

class Entity;
class Scene
{
private:
    entt::registry m_Registry;

public:
    Scene();
    ~Scene();
    Entity CreateEntity(const std::string &name = std::string());
    Entity CreateEntity(uint32_t id);
    void DestroyEntity(Entity entity);
    std::vector<Entity> GetEntities();

    friend class Entity;
};
