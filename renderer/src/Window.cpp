#include "Window.h"
#include "OpenGLWindow.h"
#ifdef __EMSCRIPTEN__
#include "WebWindow.h"
#endif
#include <string>

std::shared_ptr<Window> Window::Create(const std::string &title, const uint32_t width, const uint32_t height)
{
#ifdef __EMSCRIPTEN__
    return std::make_shared<WebWindow>(title, width, height);
#endif
    return std::make_shared<OpenGLWindow>(title, width, height);
}
