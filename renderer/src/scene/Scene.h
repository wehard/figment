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
public:
    Scene() = default;
    Scene(uint32_t width, uint32_t height);
    ~Scene();
    void OnUpdate(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize);
    void OnResize(uint32_t width, uint32_t height);
    Entity CreateEntity(const std::string &name = std::string());
    void DestroyEntity(Entity entity);
    std::vector<Entity> GetEntities();
    Entity GetEntityById(uint32_t id);
    Entity GetHoveredEntity();
    std::shared_ptr<CameraController> GetEditorCameraController();
    int32_t m_HoveredId = -1;

    std::shared_ptr<CameraController> GetActiveCameraController();
    void SetActiveCameraController(std::shared_ptr<CameraController> camera);

    friend class Entity;
private:
    entt::registry m_Registry;
    uint32_t m_Width;
    uint32_t m_Height;
    std::shared_ptr<WebGPUContext> m_GfxContext;
    std::unique_ptr<WebGPURenderer> m_Renderer;
    std::shared_ptr<CameraController> m_EditorCameraController;
    std::shared_ptr<PerspectiveCamera> m_EditorCamera;
    std::shared_ptr<CameraController> m_ActiveCameraController;
    VerletPhysics m_VerletPhysics;
};
