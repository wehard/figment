#pragma once

#include "Scene.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "entt.hpp"
#include <string>
#include <random>

struct InfoComponent
{
    std::string m_Name;

    InfoComponent() = default;
    InfoComponent(const InfoComponent &) = default;
    InfoComponent(const std::string &name) : m_Name(name) {}
};

static std::random_device s_Device;
static std::mt19937_64 s_Engine(s_Device());
static std::uniform_int_distribution<uint64_t> s_UniformDist;

struct IDComponent
{
    uint64_t ID;
    IDComponent() : ID(s_UniformDist(s_Engine)) {}
};

struct TransformComponent
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;
    TransformComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : Position(position), Rotation(rotation), Scale(scale) {}

    // operator glm::mat4 &() { return m_Transform; }
    glm::mat4 GetTransform()
    {
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), Scale);
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), Position);
        glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
        glm::mat4 m = matTranslate * matRotate * matScale;
        return (m);
    }
};

class Entity
{
private:
    entt::entity m_Handle;
    Scene *m_Scene;

public:
    Entity();
    Entity(entt::entity handle, Scene *scene);

    template <typename T, typename... Args>
    T &AddComponent(Args &&...args)
    {
        return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
    }

    template <typename T>
    T &GetComponent()
    {
        return m_Scene->m_Registry.get<T>(m_Handle);
    }

    template <typename T>
    void RemoveComponent()
    {
        m_Scene->m_Registry.remove<T>(m_Handle);
    }

    template <typename T>
    bool HasComponent()
    {
        return m_Scene->m_Registry.any_of<T>(m_Handle);
    }

    operator entt::entity() { return m_Handle; }
};
