#pragma once

#include "Framebuffer.h"
#include <GL/glew.h>
#include <vector>
#include <glm/vec4.hpp>

struct FramebufferDesc
{
    GLuint m_Width;
    GLuint m_Height;
};

struct FramebufferTextureDesc
{
    GLint m_InternalFormat;
    GLenum m_Format;
    GLenum m_Type;
};

enum class TextureFormat
{
    RGBA8 = GL_RGBA8,
    RED_INTEGER = GL_RED_INTEGER,
    Depth = GL_DEPTH24_STENCIL8

};

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(uint32_t width, uint32_t height);
    ~OpenGLFramebuffer() override;
    void Recreate();
    void Bind() override;
    void Unbind() override;
    void ClearAttachment(uint32_t index, glm::vec4 clearColor, int clearId) override;
    void Resize(uint32_t width, uint32_t height) override;
    uint32_t GetAttachmentId(uint32_t index) override;
    int GetPixel(uint32_t attachmentIndex, int x, int y) override;

private:
    uint32_t m_Width;
    uint32_t m_Height;
    GLuint m_FboID;
    GLuint rbo_id;

    std::vector<FramebufferTextureDesc> m_ColorAttachmentDescs;
    FramebufferTextureDesc m_DepthAttachmentDesc;

    std::vector<GLuint> m_ColorAttachments;
    GLuint m_DepthAttachment;
};
