#pragma once

#include "PerspectiveCamera.h"

namespace Figment
{
    class CameraController
    {
    public:
        CameraController(std::shared_ptr<PerspectiveCamera> camera);
        ~CameraController() = default;

        void Update(float deltaTime);
        std::shared_ptr<PerspectiveCamera> GetCamera();
        bool IsFpsCamera() const
        { return m_FpsCamera; };
        float GetSpeed() const
        { return m_MovementSpeed; };
        float GetRotationSpeed() const
        { return m_RotationSpeed; };
        void SetMovementSpeed(float speed);
    private:
        void ProcessKeyboard(float deltaTime);
        void ProcessMouseMovement(bool constrainPitch = true);
        void ProcessMouseScroll();
        std::shared_ptr<PerspectiveCamera> m_Camera;
        float m_MovementSpeed = 1.0;
        float m_RotationSpeed = 0.1;
        bool m_FpsCamera = false;
    };
}
