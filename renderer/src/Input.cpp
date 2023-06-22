#include "Input.h"
#include "GLFW/glfw3.h"

GLFWwindow *Input::m_Window = nullptr;
std::unordered_map<int, bool> Input::m_KeyState;
std::unordered_map<int, bool> Input::m_PrevKeyState;
std::unordered_map<int, bool> Input::m_ButtonState;
std::unordered_map<int, bool> Input::m_PrevButtonState;
glm::vec2 Input::m_MousePosition;
glm::vec2 Input::m_PrevMousePosition;
glm::vec2 Input::m_MouseScroll;
glm::vec2 Input::m_PrevMouseScroll;

void Input::Initialize(GLFWwindow *window)
{
    Input::m_Window = window;
    glfwSetScrollCallback(window, ScrollCallback);
}

void Input::Update()
{
    m_PrevKeyState = m_KeyState;
    m_PrevButtonState = m_ButtonState;
    m_PrevMousePosition = m_MousePosition;

    for (auto &key : m_KeyState)
    {
        key.second = glfwGetKey(Input::m_Window, key.first) == GLFW_PRESS;
    }

    for (auto &button : m_ButtonState)
    {
        button.second = glfwGetMouseButton(Input::m_Window, button.first) == GLFW_PRESS;
    }

    double x, y;
    glfwGetCursorPos(Input::m_Window, &x, &y);
    m_MousePosition = glm::vec2(x, y);
}

bool Input::GetKey(int key)
{
    return m_KeyState[key];
}

bool Input::GetKeyDown(int key)
{
    return m_KeyState[key] && !m_PrevKeyState[key];
}

bool Input::GetKeyUp(int key)
{
    return !m_KeyState[key] && m_PrevKeyState[key];
}

bool Input::GetButton(int button)
{
    return m_ButtonState[button];
}

bool Input::GetButtonDown(int button)
{
    return m_ButtonState[button] && !m_PrevButtonState[button];
}

bool Input::GetButtonUp(int button)
{
    return !m_ButtonState[button] && m_PrevButtonState[button];
}

glm::vec2 Input::GetMousePosition()
{
    return m_MousePosition;
}

glm::vec2 Input::GetMouseDelta()
{
    return glm::vec2(m_MousePosition.x - m_PrevMousePosition.x, m_MousePosition.y - m_PrevMousePosition.x);
}

glm::vec2 Input::GetMouseScroll()
{
    return m_MouseScroll;
}

glm::vec2 Input::GetScrollDelta()
{
    auto scrollDelta = m_MouseScroll;
    m_MouseScroll = glm::vec2(0);
    return scrollDelta;
}

void Input::ScrollCallback(GLFWwindow *w, double xOffset, double yOffset)
{
    m_MouseScroll = glm::vec2(xOffset, yOffset);
}
