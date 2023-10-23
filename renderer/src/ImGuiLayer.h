#pragma once

#include "Layer.h"
#include "Scene.h"
#include "Entity.h"
#include <string>

namespace Figment
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
    private:
        Scene *m_Scene;
        Entity m_SelectedEntity;

        void SelectEntity(Entity entity);
    };
}
