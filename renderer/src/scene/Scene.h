#pragma once

#include "GLRenderer.h"
#include "OrthoCamera.h"

#include "entt.hpp"
#include <vector>

class Entity;
class Scene
{
private:
    GLRenderer *m_Renderer;
    OrthoCamera m_Camera;
    entt::registry m_Registry;
    glm::vec4 m_ClearColor;

    uint32_t m_Width;
    uint32_t m_Height;

public:
    int m_HoveredId;
    Scene();
    Scene(uint32_t width, uint32_t height);
    ~Scene();
    Entity CreateEntity(const std::string &name = std::string());
    Entity CreateEntity(uint32_t id);
    void DestroyEntity(Entity entity);
    std::vector<Entity> GetEntities();
    Entity GetHoveredEntity();

    void Update(float deltaTime, glm::vec2 mousePosition);
    OrthoCamera &GetCamera();
    void OnResize(uint32_t width, uint32_t height);

    friend class Entity;
};
