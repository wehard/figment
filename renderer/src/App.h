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
private:
  std::shared_ptr<Window> m_Window;
  std::unique_ptr<WebGPURenderer> m_Renderer;
  std::unique_ptr<WebGPUGUIContext> m_GUICtx;
  Scene *m_Scene;

  ImVec4 m_ClearColor = ImVec4(0.15, 0.15, 0.15, 1.00f);

  bool m_FpsCamera;
  bool m_HandleEvents;
  double m_CurrentTime = 0;
  double m_LastTime = 0;

  uint32_t m_SelectedEntityId = 0;
  void HandleKeyboardInput();
  void HandleMouseInput();

public:
  App(float width, float height);
  ~App();

  void Update();
  void GUIUpdate();

  void OnResize(uint32_t width, uint32_t height);
  void UpdateShader(const char *vertSource, const char *fragSource);

  void SelectEntity(Entity entity);
  void DeleteEntity(Entity entity);
};
