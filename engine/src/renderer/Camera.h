#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace figment
{
enum class CameraType
{
    Undefined,
    Orthographic,
    Perspective
};

class Camera
{
    enum class CameraDirection
    {
        Forward,
        Backward,
        Left,
        Right
    };

public:
    Camera(CameraType cameraType): m_CameraType(cameraType) {}
    virtual ~Camera() = default;
    static std::shared_ptr<Camera> Create(CameraType cameraType);
    static std::shared_ptr<Camera> CreateOrthographicCamera(float width, float height);
    static std::shared_ptr<Camera> CreatePerspectiveCamera(float aspectRatio);

    virtual void Update()                                                                  = 0;
    virtual void Resize(float width, float height)                                         = 0;
    virtual glm::mat4 GetProjectionMatrix()                                                = 0;
    virtual glm::mat4 GetViewMatrix()                                                      = 0;
    virtual float GetAspectRatio()                                                         = 0;
    virtual glm::vec3 GetPosition()                                                        = 0;
    virtual void SetPosition(const glm::vec3& position)                                    = 0;
    virtual glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) = 0;
    CameraType GetType() const { return m_CameraType; }

private:
    CameraType m_CameraType = CameraType::Undefined;
};
} // namespace figment
