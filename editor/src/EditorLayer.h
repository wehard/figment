#pragma once

#include "Core.h"
#include "Layer.h"
#include "Scene.h"
#include "Entity.h"
#include <string>

namespace Figment
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        ~EditorLayer() override;
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(AppEvent event, void *eventData) override;
    private:
        SharedPtr<WebGPUContext> m_Context;
        UniquePtr<Scene> m_Scene;
        Entity m_SelectedEntity;

        void SelectEntity(Entity entity);
        void DrawEntityInspectorPanel(Entity entity);
        void DrawScenePanel(const std::vector<Entity> &entities,
                const std::function<void(Entity)> &selectEntity = nullptr);
        template<typename T>
        void DisplayAddComponentEntry(const std::string &entryName);
    };
}
