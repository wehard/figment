#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace figment
{
class Input
{
public:
    static void Initialize(GLFWwindow* window);
    static void Update();
    static bool GetKey(int key);
    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);
    static bool GetButton(int button);
    static bool GetButtonDown(int button);
    static bool GetButtonUp(int button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();
    static glm::vec2 GetMouseScroll();
    static glm::vec2 GetScrollDelta();

    static void HideCursor();
    static void ShowCursor();

private:
    static GLFWwindow* m_Window;
    static std::unordered_map<int, bool> m_KeyState;
    static std::unordered_map<int, bool> m_PrevKeyState;
    static std::unordered_map<int, bool> m_ButtonState;
    static std::unordered_map<int, bool> m_PrevButtonState;
    static glm::vec2 m_MousePosition;
    static glm::vec2 m_PrevMousePosition;
    static glm::vec2 m_MouseScroll;
    static glm::vec2 m_PrevMouseScroll;
    static glm::vec2 m_MouseDelta;
    static void ScrollCallback(GLFWwindow* w, double xOffset, double yOffset);
    static void MouseCallback(GLFWwindow* window, double x, double y);
};
} // namespace figment
