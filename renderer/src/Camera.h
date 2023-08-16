#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraType
{
    Ortho,
    Perspective
};

enum class CameraDirection
{
    Forward,
    Backward,
    Left,
    Right
};

class Camera
{
public:
    virtual ~Camera() = default;
    static std::shared_ptr<Camera> Create(CameraType cameraType);
    static std::shared_ptr<Camera> CreateOrthoCamera(float width, float height);
    static std::shared_ptr<Camera> CreatePerspectiveCamera(float aspectRatio);

    virtual void Update() = 0;
    virtual void Resize(float width, float height) = 0;
    virtual glm::mat4 GetProjectionMatrix() = 0;
    virtual glm::mat4 GetViewMatrix() = 0;
    virtual float GetAspectRatio() = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) = 0;
};
