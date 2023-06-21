#pragma once

#include "Entity.h"
#include "Framebuffer.h"
#include "Window.h"
#include "GUIContext.h"
#include "PerspectiveCamera.h"
#include "Scene.h"

#include <memory>

class App
{
private:
  std::shared_ptr<Window> m_Window;
  GUIContext *m_GUICtx;
  Scene *m_Scene;

  glm::vec2 m_MousePosition = glm::vec2(0, 0);
  ImVec4 m_ClearColor = ImVec4(0.15, 0.15, 0.15, 1.00f);

  bool m_FpsCamera;
  bool m_HandleEvents;
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
  void UpdateShader(const char *vertSource, const char *fragSource);

  void SelectEntity(Entity entity);
};
