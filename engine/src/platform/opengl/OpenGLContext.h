#pragma once

#include "GfxContext.h"

struct GLFWwindow;

class OpenGLContext : public Figment::GfxContext
{
public:
    OpenGLContext(GLFWwindow *window);

    void Init() override;
    void SwapBuffers() override;

private:
    GLFWwindow *m_Window;
};
