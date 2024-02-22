#include "Asteroids.h"

Asteroids::Asteroids(const PerspectiveCamera &camera, bool enabled) : Layer("Asteroids", enabled), m_Camera(camera)
{

    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    auto context = webGpuWindow->GetContext();
    m_Renderer = Figment::CreateUniquePtr<Figment::MeshRenderer>(*context);

    auto asteroidMeshData = ModelLoader::LoadObj("res/asteroid.obj");
    auto cnv = std::vector<Vertex>();
    for (int i = 0; i < asteroidMeshData.Vertices.size(); i += 3)
    {
        auto vertex = &asteroidMeshData.Vertices[i];
        cnv.emplace_back(Vertex { glm::vec3(vertex[0], vertex[1], vertex[2]) });
    }

    m_AsteroidMesh = new Mesh(context->GetDevice(), cnv, asteroidMeshData.Indices);

    for (int i = 0; i < 1000; i++)
    {
        auto r = Random::InRing(50.0, 100.0);
        m_Asteroids.push_back(Asteroid {
                .Position = glm::vec3(r.x, r.y, 0.0),
                .Rotation = glm::vec3(Random::Float(0.0f, 360.0f), Random::Float(0.0f, 360.0f),
                        Random::Float(0.0f, 360.0f)),
                .Scale = glm::vec3(Random::Float(0.25f, 0.5f)),
                .Mesh = m_AsteroidMesh
        });
    }

    auto shipMeshData = ModelLoader::LoadObj("res/cone.obj");
    auto scnv = std::vector<Vertex>();
    for (int i = 0; i < shipMeshData.Vertices.size(); i += 3)
    {
        auto vertex = &shipMeshData.Vertices[i];
        scnv.emplace_back(Vertex { glm::vec3(vertex[0], vertex[1], vertex[2]) });
    }

    m_ShipMesh = new Mesh(context->GetDevice(), scnv, shipMeshData.Indices);
    m_Ship = Ship {
            .Position = glm::vec3(0.0, 0.0, 0.0),
            .Rotation = glm::vec3(0.0, 0.0, 0.0),
            .Scale = glm::vec3(0.1),
            .Mesh = m_ShipMesh
    };
}

Asteroids::~Asteroids()
{
    delete m_AsteroidMesh;
    delete m_ShipMesh;
}

void Asteroids::OnAttach()
{

}

void Asteroids::OnDetach()
{

}

void Asteroids::OnUpdate(float deltaTime)
{
    m_Ship.Velocity *= 0.97f;
    if (Input::GetKey(GLFW_KEY_UP))
    {
        m_Ship.Velocity += m_Ship.Forward() * m_Ship.Speed * deltaTime;
    }
    if (Input::GetKey(GLFW_KEY_DOWN))
    {
        m_Ship.Velocity += -m_Ship.Forward() * m_Ship.Speed * deltaTime;
    }
    if (Input::GetKey(GLFW_KEY_LEFT))
    {
        m_Ship.Rotation.z += m_Ship.TurnSpeed * deltaTime;
    }
    if (Input::GetKey(GLFW_KEY_RIGHT))
    {
        m_Ship.Rotation.z -= m_Ship.TurnSpeed * deltaTime;
    }
    // Limit the velocity
    if (glm::length(m_Ship.Velocity) > 1.0f)
    {
        m_Ship.Velocity = glm::normalize(m_Ship.Velocity);
    }
    m_Ship.Position += m_Ship.Velocity;
    m_Renderer->BeginFrame((Camera &)m_Camera);
    m_Renderer->Draw(*m_Ship.Mesh, m_Ship.Transform());
    for (auto &asteroid : m_Asteroids)
    {
        asteroid.Update(deltaTime);
        m_Renderer->Draw(*asteroid.Mesh, asteroid.Transform());
    }
    m_Renderer->EndFrame();
}

void Asteroids::OnImGuiRender()
{

}

void Asteroids::OnEvent(AppEvent event, void *eventData)
{

}
