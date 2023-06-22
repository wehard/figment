#include "Input.h"
#include "GLFW/glfw3.h"

GLFWwindow *Input::m_Window = nullptr;
std::unordered_map<int, bool> Input::m_KeyState;
std::unordered_map<int, bool> Input::m_PrevKeyState;
std::unordered_map<int, bool> Input::m_MouseButtonState;
glm::vec2 Input::m_MousePosition;
glm::vec2 Input::m_MouseScroll;

void Input::Initialize(GLFWwindow *window)
{
    Input::m_Window = window;
    // glfwSetKeyCallback(window, KeyCallback);
    // glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // glfwSetCursorPosCallback(window, CursorPosCallback);
    // glfwSetScrollCallback(window, ScrollCallback);
}

void Input::Update()
{
    m_PrevKeyState = m_KeyState;

    for (auto &key : m_KeyState)
    {
        key.second = glfwGetKey(Input::m_Window, key.first) == GLFW_PRESS;
    }
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

bool Input::IsButtonDown(int button)
{
    return m_MouseButtonState[button];
}

glm::vec2 Input::GetMousePosition()
{
    return m_MousePosition;
}

glm::vec2 Input::GetMouseScroll()
{
    return m_MouseScroll;
}

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        m_KeyState[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        m_KeyState[key] = false;
    }
}

void Input::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        m_MouseButtonState[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        m_MouseButtonState[button] = false;
    }
}

void Input::CursorPosCallback(GLFWwindow *w, double x, double y)
{
    m_MousePosition = glm::vec2(x, y);
}

void Input::ScrollCallback(GLFWwindow *w, double xOffset, double yOffset)
{
    m_MouseScroll = glm::vec2(xOffset, yOffset);
}
