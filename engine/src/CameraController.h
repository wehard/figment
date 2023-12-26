#pragma once

#include "Core.h"
#include "PerspectiveCamera.h"

namespace Figment
{
    class CameraController
    {
    public:
        CameraController(SharedPtr<PerspectiveCamera> camera);
        ~CameraController() = default;

        void Update(float deltaTime);
        SharedPtr<PerspectiveCamera> GetCamera();
        bool IsFpsCamera() const
        { return m_FpsCamera; };
        float GetSpeed() const
        { return m_MovementSpeed; };
        float GetRotationSpeed() const
        { return m_RotationSpeed; };
    private:
        void ProcessKeyboard(float deltaTime);
        void ProcessMouseMovement(bool constrainPitch = true);
        void ProcessMouseScroll();
        SharedPtr<PerspectiveCamera> m_Camera;
        float m_MovementSpeed = 1.0;
        float m_RotationSpeed = 0.1;
        bool m_FpsCamera = false;
    };
}
