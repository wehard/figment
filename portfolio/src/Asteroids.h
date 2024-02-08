#include "Figment.h"

using namespace Figment;

class Asteroids : public Layer
{
public:
    Asteroids(const PerspectiveCamera &camera, bool enabled);
    ~Asteroids() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(AppEvent event, void *eventData) override;
private:
    struct Asteroid
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;
        Mesh *Mesh;

        glm::mat4 Transform() const
        {
            glm::mat4 matScale = glm::scale(glm::mat4(1.0f), Scale);
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), Position);
            glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y),
                    glm::radians(Rotation.z));
            return matTranslate * matRotate * matScale;
        }

        void Update(float deltaTime)
        {
            Rotation.x += 9.0f * deltaTime;
            Rotation.y += 9.0f * deltaTime;
            Rotation.z += 1.0f * deltaTime;
        }
    };

    const PerspectiveCamera &m_Camera;
    Mesh *m_Mesh;
    UniquePtr<WebGPURenderer> m_Renderer;
    WebGPUShader *m_Shader;
    std::vector<Asteroid> m_Asteroids;
};
