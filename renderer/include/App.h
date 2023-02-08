#pragma once

#include "GLContext.h"
#include "GLObject.h"
#include "GUIContext.h"
#include "GLRenderer.h"

class App
{
private:
    GLContext *gl;
    GUIContext *gui;
    GLRenderer *renderer;
    OrthoCamera *camera;
    Shader *shader;
    Shader *gridShader;
    GLObject *grid;
    GLObject *plane;

    glm::vec2 mousePosition = glm::vec2(0, 0);
    ImVec4 m_ClearColor = ImVec4(0.15, 0.15, 0.15, 1.00f);

    std::vector<GLObject *> objects;

    bool m_handleEvents;

    // static App *s_Instance;

public:
    App(float width, float height);
    ~App();

    void InsertPlane();
    void InsertCircle();
    void HandleKeyboardInput(int key, int scancode, int action, int mods);
    // void HandleMouseInput(SDL_Event event);
    void Update();
    void GUIUpdate();

    void OnResize(float width, float height);
    void SetSDLEventEnabled(bool enabled);
};
