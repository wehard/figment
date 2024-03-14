#pragma once

#include "Figment.h"

using namespace Figment;

class MainLayer : public Figment::Layer
{
public:
    MainLayer();
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;

private:
    std::vector<Layer *> m_Layers;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    float m_BlinkTimer = 0.0f;
    ImVec4 m_BlinkTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool m_SwapColor = false;
    Layer *m_SelectedLayer = nullptr;
    CameraController *m_CameraController;
};
