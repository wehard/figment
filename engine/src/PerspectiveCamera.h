#pragma once

#include "Camera.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Figment
{
    constexpr float Yaw = -90.0f;
    constexpr float Pitch = 0.0f;

    struct CameraSettings
    {
        float AspectRatio = 1.0;
        float Fov = 45.0;
        float NearClip = 0.1;
        float FarClip = 1000.0;
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;
        glm::vec3 Position;
        glm::vec3 Forward;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        float Yaw;
        float Pitch;
    };

    class PerspectiveCamera : public Figment::Camera
    {
    public:
        PerspectiveCamera(float aspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = Yaw, float pitch = Pitch);
        ~PerspectiveCamera() override;

        void Update() override;
        void UpdateProjectionMatrix();
        void UpdateViewMatrix();
        void Resize(float width, float height) override;

        glm::mat4 GetViewMatrix() override
        { return m_ViewMatrix; };
        glm::mat4 GetProjectionMatrix() override
        { return m_ProjectionMatrix; }
        glm::vec3 GetPosition() override
        { return m_Position; }
        float GetAspectRatio() override
        { return m_AspectRatio; };

        void SetPosition(const glm::vec3 &position) override
        {
            m_Position = position;
            Update();
        }

        glm::vec3 *GetPositionPtr()
        { return &m_Position; }

        CameraSettings GetSettings() const
        {
            return { m_AspectRatio, m_Fov, m_NearClip, m_FarClip, m_ProjectionMatrix, m_ViewMatrix, m_Position,
                     m_Forward,
                     m_Up, m_Right, m_WorldUp, m_Yaw, m_Pitch };
        }

        glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) override;
        friend class CameraController;
    private:
        float m_AspectRatio = 1.0;
        float m_Fov = 45.0;
        float m_NearClip = 0.1;
        float m_FarClip = 1000.0;
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        glm::vec3 m_Position;
        glm::vec3 m_Forward;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;
    };
}
