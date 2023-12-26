#include "OpenGLContext.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

OpenGLContext::OpenGLContext(GLFWwindow *window) : m_Window(window)
{
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(m_Window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to initialize!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}
