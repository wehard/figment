#pragma once

#include "BaseWindow.h"
#include "WebGPUContext.h"
#include <string>
#include <GLFW/glfw3.h>
#include <cstdint>
#include "webgpu/webgpu.h"

namespace Figment
{
    class WebGPUWindow : public Window
    {
    public:
        WebGPUWindow(const std::string &title, uint32_t width, uint32_t height);
        ~WebGPUWindow() override;
        void Resize(Window::ResizeEventData resizeData);

    private:
        std::string m_Title;
    };
}
