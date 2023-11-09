#pragma once

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
        Scene *m_Scene;
        Entity m_SelectedEntity;

        void SelectEntity(Entity entity);
    };
}
