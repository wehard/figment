#include "Asteroids.h"

Asteroids::Asteroids(const PerspectiveCamera &camera, bool enabled) : Layer("Asteroids", enabled), m_Camera(camera)
{
    auto meshData = ModelLoader::LoadObj("res/asteroid.obj");

    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    auto context = webGpuWindow->GetContext();
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*context);

    m_Shader = new WebGPUShader(context->GetDevice(),
            *Utils::LoadFile2("res/shaders/mesh.wgsl"));

    auto cnv = std::vector<Vertex>();
    for (int i = 0; i < meshData.Vertices.size(); i += 3)
    {
        auto vertex = &meshData.Vertices[i];
        cnv.emplace_back(Vertex { glm::vec3(vertex[0], vertex[1], vertex[2]) });
    }

    m_Mesh = new Mesh(context->GetDevice(), cnv, meshData.Indices);

    for (int i = 0; i < 100; i++)
    {
        m_Asteroids.push_back(Asteroid {
                .Position = glm::vec3(Random::Float(-10.0f, 10.0f), Random::Float(-10.0f, 10.0f),
                        0.0),
                .Rotation = glm::vec3(Random::Float(0.0f, 360.0f), Random::Float(0.0f, 360.0f),
                        Random::Float(0.0f, 360.0f)),
                .Scale = glm::vec3(Random::Float(0.25f, 0.5f)),
                .Mesh = m_Mesh
        });
    }
}

Asteroids::~Asteroids()
{

}

void Asteroids::OnAttach()
{

}

void Asteroids::OnDetach()
{

}

void Asteroids::OnUpdate(float deltaTime)
{
    m_Renderer->Begin((Camera &)m_Camera);
    for (auto &asteroid : m_Asteroids)
    {
        asteroid.Update(deltaTime);
        m_Renderer->Submit(*asteroid.Mesh, asteroid.Transform(), *m_Shader);
    }
    m_Renderer->End();
}

void Asteroids::OnImGuiRender()
{

}

void Asteroids::OnEvent(AppEvent event, void *eventData)
{

}
