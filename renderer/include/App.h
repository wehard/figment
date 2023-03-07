#pragma once

#include "GLContext.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Framebuffer.h"
#include "Entity.h"
#include "Scene.h"

#include <memory>

class App
{
private:
    GLContext *gl;
    GUIContext *gui;
    Shader *shader;
    Shader *gridShader;
    std::unique_ptr<Shader> m_CircleShader;
    Shader *m_FramebufferShader;
    Scene *m_Scene;

    glm::vec2 mousePosition = glm::vec2(0, 0);
    ImVec4 m_ClearColor = ImVec4(0.15, 0.15, 0.15, 1.00f);

    // std::vector<GLObject *> objects;

    bool m_handleEvents;
    double m_CurrentTime = 0;
    double m_LastTime = 0;

    Entity m_SelectedEntity = {};

public:
    App(float width, float height);
    ~App();

    void InsertPlane();
    void InsertCircle();
    void InsertCube();
    void HandleKeyboardInput(int key, int scancode, int action, int mods);
    void HandleMouseInput(int button, int action, int mods);
    void HandleMouseScroll(double xOffset, double yOffset);
    void SetMousePosition(double x, double y);
    void Update();
    void GUIUpdate();

    void OnResize(float width, float height);
    void SetSDLEventEnabled(bool enabled);
    void UpdateShader(const char *vertSource, const char *fragSource);

    void SelectEntity(Entity entity);
};
