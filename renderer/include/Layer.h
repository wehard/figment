#pragma once

#include <string>

class Layer {
public:
  Layer(std::string &name);
  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void OnUpdate(float deltaTime) {}
  virtual void OnImGuiRender() {}
};
