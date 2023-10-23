#pragma once

#include "Entity.h"
#include "Framebuffer.h"
#include "Window.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Scene.h"
#include "imgui.h"
#include "WebGPURenderer.h"
#include "WebGPUGUIContext.h"

#include <memory>

class App
{
public:
    App(float width, float height);
    ~App();

    void Update();
    void GUIUpdate();

    void OnResize(uint32_t width, uint32_t height);

    void SelectEntity(Entity entity);
    void DeleteEntity(Entity entity);
private:
    std::shared_ptr<Window> m_Window;
    std::unique_ptr<WebGPURenderer> m_Renderer;
    std::unique_ptr<WebGPUGUIContext> m_GUICtx;
    Scene *m_Scene;

    double m_CurrentTime = 0;
    double m_LastTime = 0;

    Entity m_SelectedEntity;
    void HandleKeyboardInput();
    void HandleMouseInput();
};
