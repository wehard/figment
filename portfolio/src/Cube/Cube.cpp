#include "Cube.h"
#include "WebGPUWindow.h"

Cube::Cube(std::shared_ptr<PerspectiveCamera> camera, bool enabled) : Layer("Cube", enabled), m_Camera(camera)
{
    auto window = Figment::App::Instance()->GetWindow();
    m_Window = std::dynamic_pointer_cast<Figment::WebGPUWindow>(window);
    m_Renderer = std::make_unique<MeshRenderer>(*m_Window->GetContext<WebGPUContext>());

    std::vector<Mesh::Vertex> vertices = {
            {{ -0.5, -0.5, 0.5 }, { 0.0, 0.0 }},
            {{ 0.5, -0.5, 0.5 }, { 1.0, 0.0 }},
            {{ 0.5, 0.5, 0.5 }, { 1.0, 1.0 }},
            {{ -0.5, 0.5, 0.5 }, { 0.0, 1.0 }},
            {{ -0.5, -0.5, -0.5 }, { 0.0, 0.0 }},
            {{ 0.5, -0.5, -0.5 }, { 1.0, 0.0 }},
            {{ 0.5, 0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, 0.5, -0.5 }, { 0.0, 1.0 }}
    };

    std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            4, 5, 1, 1, 0, 4,
            3, 2, 6, 6, 7, 3
    };

    m_Mesh = std::make_unique<Mesh>(m_Window->GetContext<WebGPUContext>()->GetDevice(), vertices, indices);

    auto image = Image::Load("res/2k_earth_daymap.png");
    m_Texture = WebGPUTexture::Create(m_Window->GetContext<WebGPUContext>()->GetDevice(), image);
}

void Cube::OnAttach()
{
}

void Cube::OnDetach()
{
}

void Cube::OnUpdate(float deltaTime)
{
    auto mp = Input::GetMousePosition();
    m_Position = m_Camera->ScreenToWorldSpace(mp, glm::vec2(m_Window->GetWidth(), m_Window->GetHeight()));
    m_Rotation.x += 90.0f * deltaTime;
    m_Rotation.y += 90.0f * deltaTime;
    m_Rotation.z += 10.0f * deltaTime;
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), m_Scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), m_Position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(m_Rotation.x), glm::radians(m_Rotation.y),
            glm::radians(m_Rotation.z));
    glm::mat4 transform = matTranslate * matRotate * matScale;
    m_Renderer->BeginFrame(*m_Camera);
    m_Renderer->DrawTextured(*m_Mesh, transform, *m_Texture);
    m_Renderer->EndFrame();
}

void Cube::OnImGuiRender()
{
}

void Cube::OnEvent(Figment::AppEvent event, void *eventData)
{
    auto ev = (Figment::Window::ResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}
