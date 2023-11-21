#include "Framebuffer.h"

#pragma once

namespace Figment
{
    class WebGPUFramebuffer : public Framebuffer
    {
    public:
        WebGPUFramebuffer(uint32_t width, uint32_t height);
        ~WebGPUFramebuffer() override = default;
        void Bind() override;
        void Unbind() override;
        void Resize(uint32_t width, uint32_t height) override;
        uint32_t GetAttachmentId(uint32_t index) override;
        void ClearAttachment(uint32_t index, glm::vec4 clearColor, int clearId) override;
        int GetPixel(uint32_t attachmentIndex, int x, int y) override;
    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };
}
