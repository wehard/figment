#include <GLFW/glfw3.h>
#include "CameraController.h"
#include "Input.h"

namespace figment
{
    CameraController::CameraController(std::shared_ptr<PerspectiveCamera> camera)
            : m_Camera(camera), m_MovementSpeed(60.0), m_RotationSpeed(0.1), m_FpsCamera(false)
    {

    }

    void CameraController::Update(float deltaTime)
    {
        if (Input::GetKeyUp(GLFW_KEY_SPACE))
        {
            m_FpsCamera = !m_FpsCamera;
            m_FpsCamera ? Input::HideCursor() : Input::ShowCursor();
        }
        ProcessKeyboard(deltaTime);
        if (m_FpsCamera)
            ProcessMouseMovement();
        ProcessMouseScroll();

        m_Camera->Update();
    }

    void CameraController::ProcessKeyboard(float deltaTime)
    {
        float velocity = m_MovementSpeed * deltaTime;
        if (Input::GetKey(GLFW_KEY_W))
        {
            m_Camera->m_Position += m_Camera->m_Forward * velocity;
        }
        if (Input::GetKey(GLFW_KEY_S))
        {
            m_Camera->m_Position -= m_Camera->m_Forward * velocity;
        }
        if (Input::GetKey(GLFW_KEY_A))
        {
            m_Camera->m_Position -= m_Camera->m_Right * velocity;

        }
        if (Input::GetKey(GLFW_KEY_D))
        {
            m_Camera->m_Position += m_Camera->m_Right * velocity;
        }
    }

    void CameraController::ProcessMouseMovement(bool constrainPitch)
    {
        glm::vec2 delta = Input::GetMouseDelta();

        delta.x *= m_RotationSpeed;
        delta.y *= m_RotationSpeed;

        m_Camera->m_Yaw += delta.x;
        m_Camera->m_Pitch -= delta.y;

        if (constrainPitch)
        {
            if (m_Camera->m_Pitch > 89.0f)
                m_Camera->m_Pitch = 89.0f;
            if (m_Camera->m_Pitch < -89.0f)
                m_Camera->m_Pitch = -89.0f;
        }
    }
    void CameraController::ProcessMouseScroll()
    {
        float scrollDelta = Input::GetScrollDelta().y;
        m_Camera->m_Position += m_Camera->m_Forward * scrollDelta;
    }

    std::shared_ptr<PerspectiveCamera> CameraController::GetCamera()
    {
        return m_Camera;
    }

    void CameraController::SetMovementSpeed(float speed)
    {
        m_MovementSpeed = speed;
    }
}
