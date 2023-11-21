#pragma once

#include <glm/vec4.hpp>
#include <stdint.h>
#include <memory>

namespace Figment
{
    class Framebuffer
    {
    public:
        static std::unique_ptr<Framebuffer> Create(uint32_t width, uint32_t height);
        virtual ~Framebuffer() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual uint32_t GetAttachmentId(uint32_t index) = 0;
        virtual void ClearAttachment(uint32_t index, glm::vec4 clearColor, int clearId) = 0;
        virtual int GetPixel(uint32_t attachmentIndex, int x, int y) = 0;
    };
}
