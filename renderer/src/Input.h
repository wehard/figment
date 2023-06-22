#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <unordered_map>

class Input
{
public:
    static void Initialize(GLFWwindow *window);
    static void Update();
    static bool GetKey(int key);
    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);
    static bool IsButtonDown(int button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseScroll();

private:
    static GLFWwindow *m_Window;
    static std::unordered_map<int, bool> m_KeyState;
    static std::unordered_map<int, bool> m_PrevKeyState;
    static std::unordered_map<int, bool> m_MouseButtonState;
    static glm::vec2 m_MousePosition;
    static glm::vec2 m_MouseScroll;

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow *w, double x, double y);
    static void ScrollCallback(GLFWwindow *w, double xOffset, double yOffset);
};
