#pragma once

#include "Entity.h"
#include "Layer.h"
#include "Scene.h"
#include "Window.h"
#include <string>

namespace figment
{
class EditorLayer: public Layer
{
public:
    explicit EditorLayer(Window& window);
    ~EditorLayer() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void* eventData) override;
    std::unique_ptr<Scene>& GetScene() { return m_Scene; }

private:
    Window& m_Window;
    // std::unique_ptr<OverlayRenderer> m_OverlayRenderer;
    std::unique_ptr<Scene> m_Scene;
    Entity m_SelectedEntity;

    void SelectEntity(Entity entity);
    void DrawEntityInspectorPanel(Entity entity);
    void DrawScenePanel(const std::vector<Entity>& entities,
                        const std::function<void(Entity)>& selectEntity = nullptr);
    template<typename T>
    void DisplayAddComponentEntry(const std::string& entryName);
};
} // namespace figment
