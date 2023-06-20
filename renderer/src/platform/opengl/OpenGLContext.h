#pragma once

#include "GfxContext.h"

struct GLFWwindow;

class OpenGLContext : public GfxContext
{
public:
    OpenGLContext(GLFWwindow *window);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow *m_Window;
};
