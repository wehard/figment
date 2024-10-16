#pragma once

#include "Figment.h"
#include "OverlayRenderer.h"
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
        void OnEnable() override;
        void OnDisable() override;
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(AppEvent event, void *eventData) override;
        std::unique_ptr<Scene> &GetScene() { return m_Scene; }
    private:
        std::shared_ptr<WebGPUContext> m_Context;
        std::unique_ptr<OverlayRenderer> m_OverlayRenderer;
        std::unique_ptr<Scene> m_Scene;
        Entity m_SelectedEntity;

        void SelectEntity(Entity entity);
        void DrawEntityInspectorPanel(Entity entity);
        void DrawScenePanel(const std::vector<Entity> &entities,
                const std::function<void(Entity)> &selectEntity = nullptr);
        template<typename T>
        void DisplayAddComponentEntry(const std::string &entryName);
    };
}
