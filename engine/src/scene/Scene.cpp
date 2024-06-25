#include "Scene.h"
#include "Entity.h"
#include "WebGPUWindow.h"
#include "App.h"

#ifndef __EMSCRIPTEN__
#include "ScriptEngine.h"
#endif

namespace Figment
{
    Scene::Scene(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height)
    {
        m_EditorCamera = std::make_shared<PerspectiveCamera>((float)width / (float)height);
        m_EditorCameraController = std::make_shared<CameraController>(m_EditorCamera);
        m_EditorCamera->GetPositionPtr()->y = 1.0;
        m_EditorCamera->GetPositionPtr()->z = 15.0;
        SetActiveCameraController(m_EditorCameraController);

        auto m_Window = App::Instance()->GetWindow();
        auto webGpuWindow = std::dynamic_pointer_cast<WebGPUWindow>(m_Window);
        m_Renderer = std::make_unique<ShapeRenderer>(*webGpuWindow->GetContext<WebGPUContext>());
    }

    Scene::~Scene()
    {

    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        Entity entity = { m_Registry.create(), this };
        auto &info = entity.AddComponent<InfoComponent>();
        info.m_Name = name.empty() ? "Unnamed" : name;

        entity.AddComponent<IdComponent>();
        entity.AddComponent<TransformComponent>();

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
            entities.emplace_back(e, this);
        }
        return entities;
    }

    Entity Scene::GetEntityById(uint32_t id)
    {
        if (id < 0)
            return {};
        if (m_Registry.valid((entt::entity)id))
            return { (entt::entity)id, this };
        printf("Failed to find entity with id %u\n", id);
        return {};
    }

    Entity Scene::GetHoveredEntity()
    {
        if (m_HoveredId < 0)
            return {};
        return GetEntityById(m_HoveredId);
    }

    void Scene::OnUpdate(float deltaTime, glm::vec2 mousePosition, glm::vec2 viewportSize)
    {
        m_Renderer->Begin(*m_ActiveCameraController->GetCamera()); // TODO: Make static
        m_ActiveCameraController->Update(deltaTime);

        glm::vec2 normalized = glm::vec2(mousePosition.x / viewportSize.x, mousePosition.y / viewportSize.y);
        if (m_ActiveCameraController->IsFpsCamera() || normalized.x < -1.0 || normalized.x > 1.0 || normalized.y < -1.0
                || normalized.y > 1.0)
        {
            m_HoveredId = -1;
        }
        else
        {
            m_Renderer->ReadPixel((int)mousePosition.x, (int)mousePosition.y, [this](int32_t id)
            {
                m_HoveredId = id;
            });
        }

        for (auto &e : m_Registry.view<AnimateComponent>())
        {
            Entity entity = { e, this };
            auto &animate = entity.GetComponent<AnimateComponent>();
            animate.Update(deltaTime);
        }

        auto entities = m_Registry.view<TransformComponent>();
        for (auto e : entities)
        {
            Entity entity = { e, this };
            auto &transform = entity.GetComponent<TransformComponent>();
            if (entity.HasComponent<CircleComponent>())
            {
                auto &circle = entity.GetComponent<CircleComponent>();
                auto color = entity.GetHandle() == m_HoveredId ? glm::vec4(1.0, 1.0, 1.0, 1.0) : circle.Color;
                m_Renderer->SubmitCircle(transform.Position, transform.Scale, color, (int)entity.GetHandle());
            }
            if (entity.HasComponent<QuadComponent>())
            {
                auto &quad = entity.GetComponent<QuadComponent>();
                auto color = entity.GetHandle() == m_HoveredId ? glm::vec4(1.0, 1.0, 1.0, 1.0) : quad.Color;
                m_Renderer->SubmitQuad(transform.Position, transform.Scale, color, (int)entity.GetHandle());
            }
        }
        m_Renderer->End();
    }

    std::shared_ptr<CameraController> Scene::GetEditorCameraController()
    {
        return m_EditorCameraController;
    }

    void Scene::OnResize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        m_EditorCamera->Resize((float)width, (float)height);
        auto view = m_Registry.view<CameraComponent>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto &camera = entity.GetComponent<CameraComponent>();
            camera.Controller->GetCamera()->Resize((float)width, (float)height);
        }
        m_Renderer->OnResize(width, height);
    }

    std::shared_ptr<CameraController> Scene::GetActiveCameraController()
    {
        return m_ActiveCameraController;
    }

    void Scene::SetActiveCameraController(std::shared_ptr<CameraController> controller)
    {
        m_ActiveCameraController = controller;
    }
}
