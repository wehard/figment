#pragma once

#include "Window.h"
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
        void Resize(WindowResizeEventData resizeData);

        template<class T>
        std::shared_ptr<T> GetContext()
        {
            return std::static_pointer_cast<T>(m_RenderContext);
        }

    private:
        std::string m_Title;
    };
}
