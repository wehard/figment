#pragma once

#include "Scene.h"
#include "Component.h"

#include "entt/entt.hpp"
#include <string>
#include <random>

namespace Figment
{
    class Entity
    {
    private:
        entt::entity m_Handle = entt::null;
        Scene *m_Scene = nullptr;

    public:
        Entity() = default;
        Entity(entt::entity handle, Scene *scene);
        Entity(uint32_t handle, Scene *scene);

        template<typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename T>
        T &GetComponent()
        {
            return m_Scene->m_Registry.get<T>(m_Handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            m_Scene->m_Registry.remove<T>(m_Handle);
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.any_of<T>(m_Handle);
        }

        uint32_t GetHandle()
        {
            return (uint32_t)m_Handle;
        }

        bool operator==(const Entity &other) const
        {
            return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
        }

        operator bool() const
        { return m_Handle != entt::null; }
        operator entt::entity()
        { return m_Handle; }
    };
}
