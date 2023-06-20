#include "Window.h"
#include "OpenGLWindow.h"
#include <string>

std::shared_ptr<Window> Window::Create(const std::string &title, const uint32_t width, const uint32_t height)
{
    return std::make_shared<OpenGLWindow>(title, width, height);
}
