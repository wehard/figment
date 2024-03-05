#pragma once

#include "Core.h"
#include "ShapeRenderer.h"
#include "CameraController.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "VerletPhysics.h"

#include "entt.hpp"
#include <vector>

namespace Figment
{
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
        SharedPtr<CameraController> GetEditorCameraController();
        int32_t m_HoveredId = -1;

        SharedPtr<CameraController> GetActiveCameraController();
        void SetActiveCameraController(SharedPtr<CameraController> camera);

        friend class Entity;
    private:
        entt::registry m_Registry;
        uint32_t m_Width;
        uint32_t m_Height;
        SharedPtr<WebGPUContext> m_GfxContext;
        UniquePtr<ShapeRenderer> m_Renderer;
        SharedPtr<CameraController> m_EditorCameraController;
        SharedPtr<PerspectiveCamera> m_EditorCamera;
        SharedPtr<CameraController> m_ActiveCameraController;
        VerletPhysics m_VerletPhysics;
    };
}
