#include "Entity.h"

namespace figment
{
    Entity::Entity(entt::entity handle, Scene *scene) : m_Handle(handle), m_Scene(scene)
    {
    }

    Entity::Entity(uint32_t handle, Scene *scene)
    {
        m_Handle = (entt::entity)handle;
        m_Scene = scene;
    }
}
