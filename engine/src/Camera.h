#pragma once

#include "Core.h"
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Figment
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
        Camera(CameraType cameraType) : m_CameraType(cameraType)
        {
        }
        virtual ~Camera() = default;
        static Figment::SharedPtr<Camera> Create(CameraType cameraType);
        static Figment::SharedPtr<Camera> CreateOrthographicCamera(float width, float height);
        static Figment::SharedPtr<Camera> CreatePerspectiveCamera(float aspectRatio);

        virtual void Update() = 0;
        virtual void Resize(float width, float height) = 0;
        virtual glm::mat4 GetProjectionMatrix() = 0;
        virtual glm::mat4 GetViewMatrix() = 0;
        virtual float GetAspectRatio() = 0;
        virtual glm::vec3 GetPosition() = 0;
        virtual glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) = 0;
        CameraType GetType() const
        { return m_CameraType; }
    private:
        CameraType m_CameraType = CameraType::Undefined;
    };
}
