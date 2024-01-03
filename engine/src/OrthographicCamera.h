#pragma once

#include "Camera.h"
#include <glm/glm.hpp>

namespace Figment
{
    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera();
        OrthographicCamera(float width, float height);

        void Update() override;
        void Resize(float width, float height) override;
        glm::mat4 GetProjectionMatrix() override
        { return m_ProjectionMatrix; }
        glm::mat4 GetViewMatrix() override
        { return m_ViewMatrix; }
        float GetAspectRatio() override
        { return m_AspectRatio; };
        glm::vec3 GetPosition() override
        { return m_Position; }
        glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) override;

        void SetPosition(const glm::vec3 &position)
        { m_Position = position; UpdateViewMatrix(); }
        void SetZoom(float zoom)
        { m_Zoom = zoom; SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom); UpdateViewMatrix(); }
        void BeginPan(glm::vec2 mousePosition);
        void EndPan();
    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

        float m_ViewportWidth;
        float m_ViewportHeight;
        float m_AspectRatio;
        float m_Zoom = 1.0f;
        float m_Rotation = 0.0f;
        bool m_IsPanning = false;
        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        void SetProjection(float left, float right, float bottom, float top);
        void UpdateViewMatrix();
    };
}
