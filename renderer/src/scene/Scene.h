#pragma once

#include "WebGPURenderer.h"
#include "CameraController.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "VerletPhysics.h"

#include "entt.hpp"
#include <vector>

class Entity;

class Scene
{
private:
    std::shared_ptr<CameraController> m_CameraController;
    std::shared_ptr<PerspectiveCamera> m_Camera;

    entt::registry m_Registry;

    uint32_t m_Width;
    uint32_t m_Height;

    VerletPhysics m_VerletPhysics;
    std::shared_ptr<WebGPUContext> m_GfxContext;
    std::unique_ptr<WebGPURenderer> m_Renderer;

public:
    glm::vec4 m_ClearColor;
    int32_t m_HoveredId = -1;
    Scene();
    Scene(uint32_t width, uint32_t height);
    ~Scene();
    Entity CreateEntity(const std::string &name = std::string());
    void DestroyEntity(Entity entity);
    std::vector<Entity> GetEntities();
    Entity GetEntityById(uint32_t id);
    Entity GetHoveredEntity();

    void Update(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize);
    std::shared_ptr<CameraController> GetCameraController();
    void OnResize(uint32_t width, uint32_t height);

    friend class Entity;
};
