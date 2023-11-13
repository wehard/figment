#include "WebGPUFramebuffer.h"
#include "webgpu/webgpu.h"
#include "webgpu/webgpu_cpp.h"

WebGPUFramebuffer::WebGPUFramebuffer(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
{
}

void WebGPUFramebuffer::Bind()
{

}

void WebGPUFramebuffer::Unbind()
{

}

void WebGPUFramebuffer::Resize(uint32_t width, uint32_t height)
{

}

uint32_t WebGPUFramebuffer::GetAttachmentId(uint32_t index)
{
    return 0;
}

void WebGPUFramebuffer::ClearAttachment(uint32_t index, glm::vec4 clearColor, int clearId)
{

}

int WebGPUFramebuffer::GetPixel(uint32_t attachmentIndex, int x, int y)
{
    return 0;
}
