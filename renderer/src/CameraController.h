#pragma once
#include "PerspectiveCamera.h"

class CameraController
{
public:
    CameraController(std::shared_ptr<PerspectiveCamera> camera);
    ~CameraController() = default;

    void Update(float deltaTime);

    std::shared_ptr<PerspectiveCamera> GetCamera();
private:
    void ProcessKeyboard(float deltaTime);
    void ProcessMouseMovement(bool constrainPitch = true);
    void ProcessMouseScroll();
    std::shared_ptr<PerspectiveCamera> m_Camera;
    float m_MovementSpeed = 1.0;
    float m_RotationSpeed = 0.1;
    bool m_FpsCamera = false;
};
