#pragma once

#include "Figment.h"

using namespace Figment;

class Cube : public Layer
{
public:
    Cube(std::shared_ptr<PerspectiveCamera> camera, bool enabled);
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Figment::AppEvent event, void *eventData) override;

private:
    std::shared_ptr<WebGPUWindow> m_Window = nullptr;
    std::unique_ptr<MeshRenderer> m_Renderer;
    std::shared_ptr<PerspectiveCamera> m_Camera;
    std::unique_ptr<Mesh> m_Mesh;
    WebGPUTexture *m_Texture;

    glm::vec3 m_Position = glm::vec3(0.0);
    glm::vec3 m_Rotation = glm::vec3(0.0);
    glm::vec3 m_Scale = glm::vec3(0.25);
};
