#pragma once

#include "Figment.h"

using namespace Figment;

class Cube : public Layer
{
private:
    UniquePtr<WebGPURenderer> m_Renderer;
    SharedPtr<PerspectiveCamera> m_Camera;
    Figment::Mesh *m_Mesh;
    WebGPUShader *m_Shader;
    glm::vec3 m_Position = glm::vec3(0.0);
    glm::vec3 m_Rotation = glm::vec3(0.0);
    glm::vec3 m_Scale = glm::vec3(0.25);
public:
    Cube(SharedPtr<PerspectiveCamera> camera, bool enabled) : Layer("Cube", enabled), m_Camera(camera)
    {
        auto m_Window = Figment::App::Instance()->GetWindow();
        auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
        m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());

        std::vector<Vertex> vertices = {
                {{ -0.5, -0.5, 0.5 }},
                {{ 0.5, -0.5, 0.5 }},
                {{ 0.5, 0.5, 0.5 }},
                {{ -0.5, 0.5, 0.5 }},
                {{ -0.5, -0.5, -0.5 }},
                {{ 0.5, -0.5, -0.5 }},
                {{ 0.5, 0.5, -0.5 }},
                {{ -0.5, 0.5, -0.5 }}
        };

        std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0,
                1, 5, 6, 6, 2, 1,
                7, 6, 5, 5, 4, 7,
                4, 0, 3, 3, 7, 4,
                4, 5, 1, 1, 0, 4,
                3, 2, 6, 6, 7, 3
        };

        m_Mesh = new Figment::Mesh(webGpuWindow->GetContext()->GetDevice(), vertices, indices);
        m_Shader = new WebGPUShader(webGpuWindow->GetContext()->GetDevice(),
                *Utils::LoadFile2("res/shaders/wgsl/mesh.wgsl"));
    }

    void OnAttach() override
    {

    }

    void OnDetach() override
    {

    }

    void OnUpdate(float deltaTime) override
    {
        m_Rotation.x += 90.0f * deltaTime;
        m_Rotation.y += 90.0f * deltaTime;
        m_Rotation.z += 10.0f * deltaTime;
        m_Renderer->Begin(*m_Camera);
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), m_Scale);
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), m_Position);
        glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(m_Rotation.x), glm::radians(m_Rotation.y),
                glm::radians(m_Rotation.z));
        glm::mat4 transform = matTranslate * matRotate * matScale;
        m_Renderer->Submit(*m_Mesh, transform, *m_Shader);
        m_Renderer->End();
    }

    void OnImGuiRender() override
    {
    }

    void OnEvent(Figment::AppEvent event, void *eventData) override
    {
        auto ev = (Figment::WindowResizeEventData *)eventData;
        m_Renderer->OnResize(ev->Width, ev->Height);
    }
};